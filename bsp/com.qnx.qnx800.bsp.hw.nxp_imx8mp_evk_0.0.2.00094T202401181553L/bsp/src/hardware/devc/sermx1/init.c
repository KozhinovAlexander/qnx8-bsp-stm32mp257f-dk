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
#include <sys/mman.h>
#include <string.h>

void
create_device(const TTYINIT *const dip)
{
    DEV_MX1     *dev;
    /*
     * Get a device entry and the input/output buffers for it.
     */
    dev = calloc(1, sizeof(*dev));
    if (dev == NULL) {
        perror("MX1 UART: Unable to allocate device entry\n");
        exit(1);
    }

    /*
     * Get buffers.
     */
    dev->tty.ibuf.size = dip->isize;
    dev->tty.ibuf.buff = malloc((size_t)dev->tty.ibuf.size);
    if (dev->tty.ibuf.buff == NULL) {
        perror("error failed to allocate input buffer\n");
        goto fail;
    }
    dev->tty.ibuf.head = dev->tty.ibuf.buff;
    dev->tty.ibuf.tail = dev->tty.ibuf.buff;


    dev->tty.obuf.size = dip->osize;
    dev->tty.obuf.buff = malloc((size_t)dev->tty.obuf.size);
    if (dev->tty.obuf.buff == NULL) {
        perror("error failed to allocate output buffer\n");
        goto fail;
    }
    dev->tty.obuf.head = dev->tty.obuf.buff;
    dev->tty.obuf.tail = dev->tty.obuf.buff;

    dev->tty.cbuf.size = dip->csize;
    dev->tty.cbuf.buff = malloc((size_t)dev->tty.cbuf.size);
    if (dev->tty.cbuf.buff == NULL) {
        perror("error failed to allocate canonical buffer\n");
        goto fail;
    }
    dev->tty.cbuf.head = dev->tty.cbuf.buff;
    dev->tty.cbuf.tail = dev->tty.cbuf.buff;

    dev->tty.highwater = dev->tty.ibuf.size - ((dev->tty.ibuf.size < 128) ? (dev->tty.ibuf.size/4) : 100);

    strlcpy(dev->tty.name, dip->name, sizeof(dev->tty.name));

    dev->tty.baud = dip->baud;

    /*
     * The i.MX SOCs don't technically require the LOSES_TX_INTR flag,
     * but the timer mechanism acts as a failsafe in case we ever miss a TX interrupt.
     */
    dev->tty.flags   = EDIT_INSERT | LOSES_TX_INTR;
    dev->tty.c_cflag = dip->c_cflag;
    dev->tty.c_iflag = dip->c_iflag;
    dev->tty.c_lflag = dip->c_lflag;
    dev->tty.c_oflag = dip->c_oflag;
    dev->tty.lflags = dip->lflags;
    if (dip->logging_path[0] != '\0') {
        dev->tty.logging_path = strdup(dip->logging_path);
    }

    dev->tty.verbose = dip->verbose;
    dev->tty.fifo    = (unsigned char)dip->fifo;

    dev->fifo        = (unsigned)dip->fifo;
    dev->intr        = (int)dip->intr;
    dev->clk         = (unsigned)dip->clk;
    dev->div         = (unsigned)dip->div;

    /*
     * Map device registers
     */
    dev->base = mmap_device_io(MX1_UART_SIZE, dip->port);
    if (dev->base == (uintptr_t)MAP_FAILED) {
        perror("MX1 UART: MAP_FAILED\n");
        goto fail;
    }

    /*
    * Initialize termios cc codes to an ANSI terminal.
    */
    ttc(TTC_INIT_CC, &dev->tty, 0);

    /*
    * Initialize the device's name.
    * Assume that the basename is set in device name.  This will attach
    * to the path assigned by the unit number/minor number combination
    */
    ttc(TTC_INIT_TTYNAME, &dev->tty, (int)(SET_NAME_NUMBER(dip->unit) | NUMBER_DEV_FROM_USER));

    /* Assert DSR/DTR */
    out32 ( dev->base + MX1_UART_CR3, in32(dev->base + MX1_UART_CR3) | MX1_UCR3_DSR);

    /* Clear UART configuration, which won't be cleared by setting UART device */
    out32(dev->base + MX1_UART_CR1, 0);
    out32(dev->base + MX1_UART_CR2, 0);

    /*
    * Only setup IRQ handler for non-pcmcia devices.
    * Pcmcia devices will have this done later when card is inserted.
    */
    if ((dip->port != 0) && (dev->intr != -1)) {
        ser_stty(dev);
        ser_attach_intr(dev);
    }

    /*
     * Enable UART
     */
    out32(dev->base + MX1_UART_CR1, in32(dev->base + MX1_UART_CR1) | MX1_UCR1_UARTEN);

    /*
    * Attach the resource manager
    */
    ttc(TTC_INIT_ATTACH, &dev->tty, 0);

    return;

fail:
    free(dev->tty.obuf.buff);
    free(dev->tty.ibuf.buff);
    free(dev->tty.cbuf.buff);
    free (dev);
    exit(1);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devc/sermx1/init.c $ $Rev: 987518 $")
#endif
