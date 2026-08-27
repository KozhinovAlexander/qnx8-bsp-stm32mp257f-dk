/*
 * $QNXLicenseC:
 * Copyright 2017-2019, 2022 BlackBerry Limited.
 * Copyright 2018 NXP
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

#include <stdbool.h>
#include <startup.h>
#include "aarch64/aarch64_tlb.h"
#include "board.h"
#include "imx_startup.h"

/**
 * i.MX startup source file.
 *
 * @file       board_sysctl.c
 * @addtogroup startup
 * @{
 */

/*
 * Enable/disable the various system controls.
 * This code is hardware dependant and may have to be changed
 * changed by end users.
 */

static uint64_t aarch64_tlb[TLB_SIZE] __attribute__ ((aligned(64 * 1024)));

static aarch64_tlb_t board_tlb[] = {
    {
        .start = 0,
        .len   = 0,
        .attr  = (0x04 << 2),
    },
    {
        .start = -1, .len = -1, .attr = 0,
    },
};

void imx_board_mmu_enable(unsigned base, unsigned size)
{
    board_tlb[0].start = base;
    board_tlb[0].len = size;

    aarch64_setup_tlb(board_tlb, aarch64_tlb);
    aarch64_enable_mmu((uint64_t)aarch64_tlb);
    board_icache_enable();
    board_dcache_enable();
}

void board_mmu_disable(void)
{
    board_icache_disable();
    board_dcache_disable();
    aarch64_disable_mmu();
}

void board_alignment_check_enable(void)
{
    aarch64_alignment_check_enable();
}

void board_alignment_check_disable(void)
{
    aarch64_alignment_check_disable();
}

void board_dcache_enable(void)
{
    aarch64_dcache_enable();
}

void board_dcache_disable(void)
{
    aarch64_dcache_disable();
}

void board_icache_enable(void)
{
    aarch64_icache_enable();
}

void board_icache_disable(void)
{
    aarch64_icache_disable();
}

void board_enable_caches(void)
{

}

void board_disable_caches(void)
{

}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/board_sysctl.c $ $Rev: 984580 $")
#endif
