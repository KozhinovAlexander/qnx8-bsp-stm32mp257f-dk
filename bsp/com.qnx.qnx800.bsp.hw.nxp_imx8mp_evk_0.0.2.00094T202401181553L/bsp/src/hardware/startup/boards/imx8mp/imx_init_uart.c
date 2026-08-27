/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
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

/*
 * Polled serial operations for iMX
 */

#include <startup.h>
#include <soc/nxp/imx8/common/imx_uart.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_init_uart.c
 * @addtogroup startup
 * @{
 */

/**
 * Parse UART initialization options (base address, input clock, baudrate).
 *
 * @param channel Debug device index (in debug_devices structure)
 * @param line    String line to parse.
 * @param baud    Pointer to baudrate variable.
 * @param clk     Pointer to input peripheral clock variable.
 */
static void parse_line(unsigned channel, const char *line, unsigned *baud, unsigned *clk)
{
    /* Get device base address and register stride */
    if ((*line != '.') && (*line != '\0')) {
        dbg_device[channel].base = strtoul(line, (char **)&line, 16);
        if (*line == '^') {
            dbg_device[channel].shift = strtoul(line + 1, (char **)&line, 0);
        }
    }

    /* Get baud rate value */
    if (*line == '.') {
        ++line;
    }
    if ((*line != '.') && (*line != '\0')) {
        *baud = strtoul(line, (char **)&line, 0);
    }

    /* Get input device clock rate value */
    if (*line == '.') {
        ++line;
    }
    if (*line != '.' && *line != '\0') {
        *clk = strtoul(line, (char **)&line, 0);
    }
}


/**
 * Initialise one of the serial ports.
 *
 * @param channel   Debug device index (in debug_devices structure)
 * @param init      String line with configuration parameters.
 * @param defaults  String line with configuration parameters.
 */
void imx_init_uart(unsigned channel, const char *init, const char *defaults)
{
    unsigned    baud, clk, base, rfdiv, tmp;

    /*
     * Default peripheral clock rate is 96MHz
     */
    clk = 96000000;

    parse_line(channel, defaults, &baud, &clk);
    parse_line(channel, init, &baud, &clk);
    base = dbg_device[channel].base;

    if (baud == 0) {
        return;
    }

    /* Disable UART */
    out32(base + IMX_UART_UCR1, 0x00);

    /* Reset UART device */
    out32(base + IMX_UART_UCR2, 0x00);
    /* Wait for UART reset done */
    while ((in32(base + IMX_UART_UCR2) & IMX_UART_UCR2_SRST) == 0) {}

    /*
     * 8-bit, no-parity, 1 stop bit
     * ignore RTS, active CTS
     */
    out32(base + IMX_UART_UCR2, (IMX_UART_UCR2_IRTS |
                                 IMX_UART_UCR2_CTS |
                                 IMX_UART_UCR2_WS |
                                 IMX_UART_UCR2_SRST));

    /* The Reference Frequency = 16MHz */
    out32(base + IMX_UART_UCR3, 0x00);
    out32(base + IMX_UART_UCR4, 0x00);

    rfdiv = clk / 16000000;
    /* We expect clock is <= 96MHz */
    if (rfdiv) {
        clk /= rfdiv;
        rfdiv = 6 - rfdiv;
    } else {
        rfdiv = 5;
    }
    out32(base + IMX_UART_UFCR, (0x00000801 |
                                 (rfdiv << IMX_UART_UFCR_RFDIV_SHIFT)));

    tmp = (baud * 16) / (clk / 10000);
    out32(base + IMX_UART_UBIR, tmp - 1);
    out32(base + IMX_UART_UBMR, 9999);

    /* Enable UART */
    out32(base + IMX_UART_UCR1, IMX_UART_UCR1_UARTEN);

    /* Enable Tx/Rx */
    out32(base + IMX_UART_UCR2, (IMX_UART_UCR2_IRTS |
                                 IMX_UART_UCR2_CTS |
                                 IMX_UART_UCR2_WS |
                                 IMX_UART_UCR2_TXEN |
                                 IMX_UART_UCR2_RXEN |
                                 IMX_UART_UCR2_SRST));
}

/**
 * Send a character.
 *
 * @param data Character to send.
 */
void imx_uart_put_char(int data)
{
    unsigned base = dbg_device[0].base;

    while (!(in32(base + IMX_UART_USR1) & IMX_UART_USR1_TRDY)) {}
    out32(base + IMX_UART_UTXD, (unsigned)data);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_uart.c $ $Rev: 984580 $")
#endif
