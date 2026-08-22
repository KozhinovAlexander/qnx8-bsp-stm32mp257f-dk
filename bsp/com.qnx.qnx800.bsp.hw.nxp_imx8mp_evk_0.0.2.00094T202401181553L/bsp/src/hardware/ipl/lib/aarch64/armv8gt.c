/*
 * Copyright (c) 2019, 2022, 2023, BlackBerry Limited.
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
 * $
 */


#include "ipl.h"

static uint64_t clkrate;

static uint64_t asm_get_pctcnt(void) {
    uint64_t pctcnt;
    __asm__ __volatile__ ("mrs %0, cntpct_el0" : "=r" (pctcnt));
    return pctcnt;
}

static void asm_nop(void) {
    __asm__ __volatile__ ("nop");
}

static uint64_t asm_get_cntfrq(void)
{
    uint64_t cntfrq;
    __asm__ __volatile__("mrs   %0, cntfrq_el0" : "=r"(cntfrq));
    return cntfrq;
}

static void disable_cntv_intr(void)
{
    __asm__ __volatile__("msr   cntv_ctl_el0, %0" : : "r"(0));
}

static void armv8gt_udelay(const unsigned usec)
{
    uint64_t start, cnt;
    const uint64_t usec_cnt = (uint64_t)usec * (clkrate / (uint64_t)1000000);

    start = asm_get_pctcnt();
    do {
        asm_nop();
        cnt = asm_get_pctcnt();
    } while (cnt - start < usec_cnt);
}

void armv8gt_init(const uint64_t clock)
{
    if (clock != 0) {
        clkrate = clock;
    } else {
        clkrate = asm_get_cntfrq();
    }

    /*
     * Disable CNTV interrupt
     */
    disable_cntv_intr();

    init_udelay(armv8gt_udelay);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/armv8gt.c $ $Rev: 975396 $")
#endif
