/*
 * Copyright (c) 2016,2023 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
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

#include "ipl.h"
#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_uart.h>
#include "imx_ipl.h"

/**
 * i.MX IPL source file.
 *
 * @file       imx_uart.c
 * @addtogroup ipl
 * @{
 */

/* UART function prototype */
static uint8_t imx_uart_pollkey(void);
static uint8_t imx_uart_getchar(void);
static void imx_uart_putchar(const uint8_t data);

static const ser_dev imx_serial_device = {
    .get_byte = imx_uart_getchar,
    .put_byte = imx_uart_putchar,
    .poll = imx_uart_pollkey
};

static unsigned imx_serial_base;

/**
 *  Initializes UART device, enable transmitter and receiver.
 *
 * @param port - Base address of initialized UART device
 * @param baud - Baud rate
 * @param clk  - UART input module clock
 */
void imx_init_uart(const unsigned port, const unsigned baud, unsigned clk)
{
    unsigned tmp, rfdiv = IMX_UART_UFCR_RFDIV_2;

    /* Wait for UART to finish transmitting */
    while (!(in32(port + IMX_UART_UTS) & IMX_UART_UTS_TXEMPTY))
    {
    }

    /* Disable UART */
    tmp = in32(port + IMX_UART_UCR1) & (~IMX_UART_UCR1_UARTEN);
    out32(port + IMX_UART_UCR1, tmp);

    /* Set to default POR state */
    out32(port + IMX_UART_UCR1, 0x0000);
    out32(port + IMX_UART_UCR2, 0x0000);

    while (!(in32(port + IMX_UART_UCR2) & IMX_UART_UCR2_SRST))
    {
    }

    out32(port + IMX_UART_UCR3, 0x0704);
    out32(port + IMX_UART_UCR4, 0x8000);
    out32(port + IMX_UART_UFCR, 0x0801);
    out32(port + IMX_UART_UESC, 0x002B);
    out32(port + IMX_UART_UTIM, 0x0000);
    out32(port + IMX_UART_UBIR, 0x0000);
    out32(port + IMX_UART_UBMR, 0x0000);
    out32(port + IMX_UART_ONEMS, 0x0000);
    out32(port + IMX_UART_UTS, 0x0000);
    /* Configure FIFOs, Default divisor (RFDIV) 2 */
    out32(port + IMX_UART_UFCR, ((1 << IMX_UART_UFCR_RXTL_SHIFT) |
                                 IMX_UART_UFCR_RFDIV_2 |
                                 (2 << IMX_UART_UFCR_TXTL_SHIFT)));

    rfdiv = clk / 16000000;
    /* We expect clock is <= 96MHz */
    if (rfdiv) {
        clk /= rfdiv;
        rfdiv = 6 - rfdiv;
    } else {
        rfdiv = 5;
    }
    out32(port + IMX_UART_UFCR, (0x00000801 |
                                 (rfdiv << IMX_UART_UFCR_RFDIV_SHIFT)));

    /* Set to 8N1 */
    tmp = in32(port + IMX_UART_UCR2) & (~IMX_UART_UCR2_PREN);
    out32(port + IMX_UART_UCR2, tmp);

    tmp = in32(port + IMX_UART_UCR2) | IMX_UART_UCR2_WS;
    out32(port + IMX_UART_UCR2, tmp);

    tmp = in32(port + IMX_UART_UCR2) & (~IMX_UART_UCR2_STPB);
    out32(port + IMX_UART_UCR2, tmp);

    /* Ignore RTS */
    tmp = in32(port + IMX_UART_UCR2) | IMX_UART_UCR2_IRTS;
    out32(port + IMX_UART_UCR2, tmp);

    /* Enable UART */
    tmp = in32(port + IMX_UART_UCR1) | IMX_UART_UCR1_UARTEN;
    out32(port + IMX_UART_UCR1, tmp);

    /* Enable FIFOs */
    tmp = in32(port + IMX_UART_UCR2) | (IMX_UART_UCR2_SRST |
                                        IMX_UART_UCR2_RXEN | IMX_UART_UCR2_TXEN);
    out32(port + IMX_UART_UCR2, tmp);

    /* Clear status flags */
    tmp = in32(port + IMX_UART_USR2) | (IMX_UART_USR2_ADET |
                                        IMX_UART_USR2_IDLE   |
                                        IMX_UART_USR2_IRINT  |
                                        IMX_UART_USR2_WAKE   |
                                        IMX_UART_USR2_RTSF   |
                                        IMX_UART_USR2_BRCD   |
                                        IMX_UART_USR2_ORE    |
                                        IMX_UART_USR2_RDR);
    out32(port + IMX_UART_USR2, tmp);

    /* Clear status flags */
    tmp = in32(port + IMX_UART_USR1) | (IMX_UART_USR1_PARITYERR |
                                        IMX_UART_USR1_RTSD   |
                                        IMX_UART_USR1_ESCF   |
                                        IMX_UART_USR1_FRAMERR    |
                                        IMX_UART_USR1_AIRINT |
                                        IMX_UART_USR1_AWAKE);
    out32(port + IMX_UART_USR1, tmp);

    tmp = (baud * 16) / (clk / 10000);
    out32(port + IMX_UART_UBIR, tmp - 1);
    out32(port + IMX_UART_UBMR, 9999);

    imx_serial_base = port;

    /*
    * Register our debug functions
    */
    init_serdev((ser_dev *)&imx_serial_device);
}

/**
 * Indicates that at least 1 character is received and
 * written to the Rx FIFO
 *
 * @return 1 - Receive data ready; 0 - No receive data ready.
 */
static unsigned char imx_uart_pollkey(void)
{
    if (in32(imx_serial_base + IMX_UART_USR2) & IMX_UART_USR2_RDR) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Wait for a receive data ready, read char from Rx FIFO.
 *
 * @return Received char.
 */
static unsigned char imx_uart_getchar(void)
{
    while (!(imx_uart_pollkey()))
    {
    }
    return ((unsigned char) in32(imx_serial_base + IMX_UART_URXD));
}

/**
 * Write char to the UART Tx FIFO.
 *
 * @param data1 - Data char to write.
 */
static void imx_uart_putchar(const unsigned char data1)
{
    while (!(in32(imx_serial_base + IMX_UART_USR1) & IMX_UART_USR1_TRDY))
    {
    }
    out32(imx_serial_base + IMX_UART_UTXD, (unsigned) data1);
}

/** @} */ /* End of ipl */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_uart.c $ $Rev: 980064 $")
#endif
