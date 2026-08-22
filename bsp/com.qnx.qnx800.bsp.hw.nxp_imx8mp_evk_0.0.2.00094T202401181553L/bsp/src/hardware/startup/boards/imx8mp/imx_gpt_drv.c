/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017-2019 NXP
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

#include <startup.h>
#include "imx_startup.h"
#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_gpt.h>
#include "board.h"

/**
 * i.MX startup source file.
 *
 * @file       imx_timer_gpt_drv.c
 * @addtogroup startup
 * @{
 */

/*
 * Note that the timer functions do not accomodate for timer rollovers, therefore
 * the functions below should not be used to time durations longer than 179 seconds.
 */


/**
 * Enable the timer if it is not already enabled
 * Note that the timer code does not handle rollovers as of now.
 */
void imx_timer_init(void)
{
    /* Disable GPT1 */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_CR, 0x00);
    /* Disable interrupt */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_IR, 0x00);
    /* Set clock source = peripheral clock */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_CR, ((IMX_GPT_CR_CLKSRC_PERIPH_CLK << IMX_GPT_CR_CLKSRC_SHIFT) |
                                         IMX_GPT_CR_EN_24M));
    /* Set SWR = 1 */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_CR, ((IMX_GPT_CR_SWR | \
                                         (IMX_GPT_CR_CLKSRC_PERIPH_CLK << IMX_GPT_CR_CLKSRC_SHIFT)) |
                                         IMX_GPT_CR_EN_24M));
    /* Set ENMOD = 1 */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_CR, ((IMX_GPT_CR_ENMOD | \
                                         (IMX_GPT_CR_CLKSRC_PERIPH_CLK << IMX_GPT_CR_CLKSRC_SHIFT)) |
                                         IMX_GPT_CR_EN_24M));
    /* Set ENMOD = 1 + EN = 1 */
    out32(IMX_GPT_DRV_BASE + IMX_GPT_CR, ((IMX_GPT_CR_ENMOD | IMX_GPT_CR_EN | \
                                         (IMX_GPT_CR_CLKSRC_PERIPH_CLK << IMX_GPT_CR_CLKSRC_SHIFT)) |
                                         IMX_GPT_CR_EN_24M));
}

/**
 * Read timer value.
 *
 * @return  Timer value.
 */
unsigned int imx_get_timer_val(void)
{
    return in32(IMX_GPT_DRV_BASE + IMX_GPT_CNT);
}

/**
 * Return the time in microseconds between readings.
 *
 * @param t_first  First sampled timer value.
 * @param t_second Second sampled timer value.
 *
 * @return         Time in microseconds between readings.
 */
unsigned int imx_get_timer_delta(unsigned int t_first, unsigned int t_second)
{
    return ((t_second - t_first) / (IMX_GPT_CLOCK_FREQ / 1000000UL));
}

/**
 * Print the time in microseconds between readings.
 * The GPT clock is 24MHz, therefore we return the time in microseconds by dividing
 * the number of clocks by 24.
 *
 * @param t_first  First sampled timer value.
 * @param t_second Second sampled timer value.
 */
void imx_timer_print_delta(unsigned int t_first, unsigned int t_second)
{
    kprintf("\n");
    kprintf("Startup: time between timer readings (decimal): %d useconds\n",
            (t_second - t_first) / (IMX_GPT_CLOCK_FREQ / 1000000UL));
}

/**
 * Delay loop for a given number of microseconds.
 *
 * @param sleep_duration Given number of microseconds.
 */
void imx_usleep(uint32_t sleep_duration)
{
    uint32_t start, dur;

    start = in32(IMX_GPT_DRV_BASE + IMX_GPT_CNT);
    dur = (sleep_duration * (IMX_GPT_CLOCK_FREQ / 1000000UL));

    while ((in32(IMX_GPT_DRV_BASE + IMX_GPT_CNT) - start) <= dur) {
        __asm__ __volatile__("nop");
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_gpt_drv.c $ $Rev: 984580 $")
#endif
