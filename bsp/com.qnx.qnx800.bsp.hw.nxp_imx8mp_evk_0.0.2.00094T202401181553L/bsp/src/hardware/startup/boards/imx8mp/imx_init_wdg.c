/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2019 NXP
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

#include <stdint.h>
#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_wdog.h>
#include "board.h"
#include <startup.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_init_wdg.c
 * @addtogroup startup
 * @{
 */

/* The watchdog timeout value should be specified in board.h.  Set default value to 30 seconds. */
#if !defined(IMX_WDOG_TIMEOUT)
    #define IMX_WDOG_TIMEOUT            30000
#endif

/* Macro for conversion from seconds to reg. value. Shift by 8 is used because of shift in register WCR */
#define IMX_WDOG_SECONDS_TO_TIMEOUT_BITS    (((IMX_WDOG_TIMEOUT / 500) - 1) << 8)

/**
 * Enable Watchdog.
 */
void imx_wdg_enable(void)
{
    out16(IMX_WDOG_BASE + IMX_WDOG_WCR, in16(IMX_WDOG_BASE + IMX_WDOG_WCR) | IMX_WDOG_WCR_WDE_MASK);
}

/**
 * Function performs initialization and start of WDOG. By default WDOG1 will be used.
 */
void imx_wdg_reload(void)
{
    uint16_t control_val;

    /* Read actual value of watchdog control register */
    control_val = in16(IMX_WDOG_BASE + IMX_WDOG_WCR);

    /* Set timeout value */
    control_val &= ~(IMX_WDOG_WCR_WT_MASK);
    control_val |= IMX_WDOG_SECONDS_TO_TIMEOUT_BITS;

    /* Only assert wdog_rst upon time-out event */
    control_val &= ~(IMX_WDOG_WCR_WDT_MASK);

    /* Suspend (disable) the watchdog timer in low-power modes (STOP and DOZE mode) */
    control_val |= IMX_WDOG_WCR_WDZST_MASK;

    /* Make sure watchdog is not enabled yet - disable it */
    control_val &= ~(IMX_WDOG_WCR_WDE_MASK);

    /* Write into watchdog control register */
    out16(IMX_WDOG_BASE + IMX_WDOG_WCR, control_val);

}

#if IMX_CHECK_RESET
/**
 * Check and print source of the last reset.
 */
void imx_check_reset_source(void)
{
    uint32_t reset_src;

    reset_src = in16(IMX_WDOG_BASE + IMX_WDOG_WRSR);
    if (reset_src) {
        kprintf("Last Reset Sources are: ");
        if (reset_src & 0x1) {
            kprintf("SFTW ");
        } else if (reset_src & 0x2) {
            kprintf("TOUT ");
        } else if (reset_src & (1 << 4)) {
            kprintf("POR");
        }
        kprintf("\n\n");
    }
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_wdg.c $ $Rev: 984580 $")
#endif
