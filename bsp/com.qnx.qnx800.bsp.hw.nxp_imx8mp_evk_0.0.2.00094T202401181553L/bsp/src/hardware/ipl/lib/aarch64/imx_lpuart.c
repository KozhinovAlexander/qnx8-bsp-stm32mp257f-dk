/*
 * Copyright (c) 2019, 2021-2023, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017-2018, NXP
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
 *
 */

#include "ipl.h"
#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_lpuart.h>

/**
 * i.MX IPL source file.
 *
 * @file       ipl/lib/aarch64/imx_lpuart.c
 * @addtogroup ipl
 * @{
 */

/* LPUART function prototype */
static char imx_lpuart_pollkey(void);
static char imx_lpuart_getchar(void);
static void imx_lpuart_putchar(char data);

static unsigned long imx_lpuart_base;

static int calculate_divisor(unsigned baud, unsigned clk, uint32_t *psbr, uint32_t *posr);

/**
 *  Initializes LPUART device, enable transmitter and receiver.
 *
 * @param port    Base address of initialized LPUART device.
 * @param baud    Baud rate.
 * @param clk     LPUART input module clock.
 * @param osr     LPUART receiver oversampling ratio (OSR 4-32).
 */
void imx_init_lpuart(const unsigned long port, const unsigned baud, const unsigned clk, unsigned osr)
{
    uint32_t    reg32;
    uint32_t    sbr = 0;
    static const ser_dev imx_dev = {
        .get_byte = imx_lpuart_getchar,
        .put_byte = imx_lpuart_putchar,
        .poll     = imx_lpuart_pollkey
    };

    /* Wait for UART to finish transmitting */
    while (!(in32(port + IMX_LPUART_STAT) & IMX_LPUART_STAT_TDRE_MASK)) {
    }

    /* Disable LPUART receiver & transmitter */
    reg32 = in32(port + IMX_LPUART_CTRL) & (~(IMX_LPUART_CTRL_RE_MASK | IMX_LPUART_CTRL_TE_MASK));
    out32(port + IMX_LPUART_CTRL, reg32);

    /* Set to default POR state */
    out32(port + IMX_LPUART_GLOBAL, IMX_LPUART_GLOBAL_RST_MASK);
    while ((in32(port + IMX_LPUART_GLOBAL) & IMX_LPUART_GLOBAL_RST_MASK) == 0) {
    }
    out32(port + IMX_LPUART_GLOBAL, 0x00);
    while ((in32(port + IMX_LPUART_GLOBAL) & IMX_LPUART_GLOBAL_RST_MASK) != 0) {
    }

    /* Set to 8N1, no parity */
    reg32 = in32(port + IMX_LPUART_CTRL);
    reg32 &= ~(IMX_LPUART_CTRL_M_MASK | IMX_LPUART_CTRL_PE_MASK | IMX_LPUART_CTRL_PT_MASK);
    out32(port + IMX_LPUART_CTRL, reg32);

    /* Set LPUART BaudRate */
    if (osr != 0) {
        /* SBR = (LPUART clock / baud rate) / (OSR + 1) */
        sbr = (clk / (baud * osr)) & IMX_LPUART_BAUD_SBR_MASK;
    } else {
        calculate_divisor(baud, clk, &sbr, &osr);
    }
    if (osr != 0) {
        reg32 = (in32(port + IMX_LPUART_BAUD) &
                ~(IMX_LPUART_BAUD_OSR_MASK | IMX_LPUART_BAUD_SBR_MASK | IMX_LPUART_BAUD_BOTHEDGE_MASK));
        reg32 |= ((osr - 1) << IMX_LPUART_BAUD_OSR_SHIFT) | sbr;
        out32(port + IMX_LPUART_BAUD, reg32);
    }

    /* Clear status flags */
    reg32 = in32(port + IMX_LPUART_STAT) |
            (IMX_LPUART_STAT_LBKDIF_MASK |
             IMX_LPUART_STAT_RXEDGIF_MASK |
             IMX_LPUART_STAT_IDLE_MASK |
             IMX_LPUART_STAT_OR_MASK |
             IMX_LPUART_STAT_NF_MASK |
             IMX_LPUART_STAT_FE_MASK |
             IMX_LPUART_STAT_PF_MASK |
             IMX_LPUART_STAT_MA1F_MASK |
             IMX_LPUART_STAT_MA2F_MASK);
    out32(port + IMX_LPUART_STAT, reg32);

    /* Enable LPUART receiver & transmitter */
    reg32 = in32(port + IMX_LPUART_CTRL);
    reg32 |= (IMX_LPUART_CTRL_RE_MASK | IMX_LPUART_CTRL_TE_MASK);
    out32(port + IMX_LPUART_CTRL, reg32);

    imx_lpuart_base = port;

    /* Register our debug functions */
    init_serdev((ser_dev *)&imx_dev);
}

/**
 * Indicates that at least 1 character is received and
 * written to the Rx FIFO
 *
 * @return 1 - Receive data ready; 0 - No receive data ready.
 */
static char imx_lpuart_pollkey(void)
{
    if (in32(imx_lpuart_base + IMX_LPUART_STAT) & IMX_LPUART_STAT_RDRF_MASK) {
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
static char imx_lpuart_getchar(void)
{
    while (imx_lpuart_pollkey() == 0) {
    }
    return ((char)(in32(imx_lpuart_base + IMX_LPUART_DATA) & IMX_LPUART_DATA_RT_MASK));
}

/**
 * Write char to the UART Tx FIFO.
 *
 * @param data1 Data char to write.
 */
static void imx_lpuart_putchar(const char data)
{
    while (!(in32(imx_lpuart_base + IMX_LPUART_STAT) & IMX_LPUART_STAT_TDRE_MASK)) {
    }
    out32(imx_lpuart_base + IMX_LPUART_DATA, (uint32_t)data);
}

static int calculate_divisor(const unsigned baud, const unsigned clk, uint32_t *psbr, uint32_t *posr)
{
    uint16_t    sbr, sbrtemp;
    uint16_t    osr, i;
    uint32_t    tempdiff, calculatedbaud, bauddiff;

    osr = 4;
    sbr = (uint16_t)(clk / (baud * osr));
    calculatedbaud = clk / (unsigned)(osr * sbr);
    if (calculatedbaud > baud) {
        bauddiff = calculatedbaud - baud;
    } else {
        bauddiff = baud - calculatedbaud;
    }

    /* Loop to find the best osr value possible, one that generates minimum baudDiff
     * iterate through the rest of the supported values of osr
     */
    for (i = 5; i <= 32; i++) {
        /* Calculate the temporary sbr value   */
        sbrtemp = (uint16_t)(clk / (baud * i));
        /* Calculate the baud rate based on the temporary osr and sbr values */
        calculatedbaud = clk / (unsigned)(i * sbrtemp);

        if (calculatedbaud > baud) {
            tempdiff = calculatedbaud - baud;
        } else {
            tempdiff = baud - calculatedbaud;
        }

        if (tempdiff <= bauddiff) {
            bauddiff = tempdiff;
            osr = i;        /* Update and store the best osr value calculated */
            sbr = sbrtemp;  /* Update store the best sbr value calculated */
        }
    }

    if (bauddiff < ((baud / 100) * 3)) {
        *psbr = sbr;
        *posr = osr;
        return 0;
    }

    *posr = 0;
    *psbr = 0;

    return (-1);
}
/** @} */ /* End of ipl */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/imx_lpuart.c $ $Rev: 975396 $")
#endif
