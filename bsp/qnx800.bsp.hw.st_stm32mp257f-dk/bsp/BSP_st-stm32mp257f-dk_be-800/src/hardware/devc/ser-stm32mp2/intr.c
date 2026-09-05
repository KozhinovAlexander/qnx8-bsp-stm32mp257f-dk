/*
 * Copyright (c) 2007-2008,2023, BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 */

#include "proto.h"

#define MX1_RXERR	(MX1_URXD_ERR | MX1_URXD_OVERRUN | MX1_URXD_FRMERR | MX1_URXD_BRK | MX1_URXD_PRERR)

static inline int ms_interrupt(DEV_MX1 *dev)
{
    int status = 0;
    const uintptr_t base = dev->base;
    uint32_t sr1;

    sr1 = in32(base + MX1_UART_SR1);
    out32(base + MX1_UART_SR1, MX1_USR1_RTSD);
    if (dev->tty.c_cflag & OHFLOW) {
        status |= tti(&dev->tty, ((sr1 & MX1_USR1_RTSS) ? TTI_OHW_CONT : TTI_OHW_STOP));
    }

    return (status);
}

static inline int tx_interrupt(DEV_MX1 *dev)
{
    int    status = 0;
    const uintptr_t base = dev->base;
    uint32_t cr1;

    cr1 = in32(base + MX1_UART_CR1);

    if (cr1 & MX1_UCR1_TXMPTYEN) {
        out32((base + MX1_UART_CR1), (cr1 & ~MX1_UCR1_TXMPTYEN));

        dev->tty.un.s.tx_tmr = 0;
        /* Send event to io-char, tto() will be processed at thread time */
        atomic_set(&dev->tty.flags, EVENT_TTO);
        status |= 1;
    }

    return (status);
}

static inline int rx_interrupt(DEV_MX1 *dev)
{
    int            status = 0;
    int            byte_count = 0;
    unsigned       key, rxdata;
    const uintptr_t base = dev->base;

    /* limit loop iterations by FIFO size to prevent ISR from running too long */
    while ((in32(base + MX1_UART_SR2) & MX1_USR2_RDR) && (byte_count < MX1_UART_FIFO_SIZE)) {
        /*
         * Read next character from FIFO
         */
        rxdata = in32(base + MX1_UART_RXDATA);
        key = rxdata & 0xFF;
        if (rxdata & MX1_RXERR) {
            /*
             * Save error as out-of-band data which can be read via devctl()
             */
            dev->tty.oband_data |= rxdata;
            atomic_set(&dev->tty.flags, OBAND_DATA);

            if (rxdata & MX1_URXD_BRK) {
                key |= TTI_BREAK;
            } else if (rxdata & MX1_URXD_FRMERR) {
                key |= TTI_FRAME;
            } else if (rxdata & MX1_URXD_PRERR) {
                key |= TTI_PARITY;
            } else if (rxdata & MX1_URXD_OVERRUN) {
                key |= TTI_OVERRUN;
            } else {
                /* do nothing */
            }
        }
        status |= tti(&dev->tty, key);
        byte_count++;
    }

    /*
     * Note that as soon the RX FIFO data level drops below the RXTL threshold
     * the Receiver Ready (RRDY) interrupt will automatically clear
     */

    /* Clear the ageing timer interrupt if it caused this interrupt */
    if (in32(base + MX1_UART_SR1) & MX1_USR1_AGTIM) {
        out32(base + MX1_UART_SR1, MX1_USR1_AGTIM);
    }

    return status;
}

static inline int do_interrupt(DEV_MX1 *const dev)
{
    int sts = 0;

    if (in32(dev->base + MX1_UART_SR1) & MX1_USR1_RTSD) {
        sts |= ms_interrupt(dev);
    }

    /*
     * If the interrupt was caused by the RX FIFO fill level
     * being reached or the aging timer interrupt fired process the RX interrupt
     */
    if (in32(dev->base + MX1_UART_SR1) & (MX1_USR1_RRDY | MX1_USR1_AGTIM)) {
        sts = rx_interrupt(dev);
    }

    if (in32(dev->base + MX1_UART_SR1) & MX1_USR1_TRDY) {
        sts |= tx_interrupt(dev);
    }

    return sts;
}

static int
interrupt_event_handler (__attribute__((unused)) message_context_t *const msgctp, __attribute__((unused)) const int code,
                         __attribute__((unused)) const unsigned flags, void *const handle)
{
    int status;
    DEV_MX1 *dev = handle;

    status = do_interrupt (dev);
    if (status) {
        iochar_send_event (&dev->tty);
    }

    InterruptUnmask (dev->intr, dev->iid);
    return (EOK);
}

void
ser_attach_intr(DEV_MX1 *dev)
{
    struct sigevent event;

    // Associate a pulse which will call the event handler.
    event.sigev_code = (short)pulse_attach (ttyctrl.dpp, MSG_FLAG_ALLOC_PULSE, 0, &interrupt_event_handler, dev);
    if (event.sigev_code == -1) {
        fprintf (stderr, "Unable to attach event pulse.%s\n", strerror(errno));
        return;
    }

    /* Init the pulse for interrupt event */
    event.sigev_notify = SIGEV_PULSE;
    event.sigev_coid = ttyctrl.coid;

    /*
     * If the interrupt is being handled by an event, then set the event priority
     * to the io-char event priority+1. The io-char event priority can be configured
     * by the "-o priority=X" parameter.
     */
    event.sigev_priority = (short)(ttyctrl.event.sigev_priority + 1);
    event.sigev_value.sival_int = 0;

    dev->iid = InterruptAttachEvent (dev->intr, &event, _NTO_INTR_FLAGS_TRK_MSK);
    if (dev->iid == -1) {
        fprintf (stderr, "Unable to attach InterruptEvent. %s\n", strerror(errno));
        return;
    }

    /* Ensure that DMA interrupts are all disabled */
    out32(dev->base + MX1_UART_CR1,
          (in32(dev->base + MX1_UART_CR1) & (~(MX1_UCR1_ATDMAEN | MX1_UCR1_RDMAEN | MX1_UCR1_TDMAEN))));

    /* Enable aging timer interrupt */
    out32(dev->base + MX1_UART_CR2, in32(dev->base + MX1_UART_CR2) | MX1_UCR2_ATEN);
}

void ser_detach_intr(DEV_MX1 *dev)
{
    const uintptr_t base = dev->base;

    /* Disable UART */
    out32(base + MX1_UART_CR1, 0x04);
    out32(base + MX1_UART_CR2, 0x00);

    InterruptDetach(dev->iid);
    dev->intr = -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devc/sermx1/intr.c $ $Rev: 982527 $")
#endif
