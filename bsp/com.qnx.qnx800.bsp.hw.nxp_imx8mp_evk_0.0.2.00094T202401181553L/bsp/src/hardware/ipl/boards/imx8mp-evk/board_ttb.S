/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017, 2022 NXP
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


/*
 * arm_board_ttb.S
 *
 */
#include "board.h"

#if !defined(IMX_SPL_BOOT)
    .data
    .globl  aarch64_arm_board_ttb

    .align  16
aarch64_arm_board_ttb:
    /* Uncacheable */
    .set    XCount, ((1 << 0) | (1 << 10))
    .rept   (IMX_SDRAM_BASE >> 29)
    .dword  XCount
    .set    XCount, XCount + 0x20000000
    .endr

    /* SDRAM, cacheable and bufferable */
    .set    XCount, (XCount | (4 << 2))
    .rept   (IMX_SDRAM_SIZE >> 29)
    .dword  XCount
    .set    XCount, XCount + 0x20000000
    .endr

    /* Uncacheable */
    .set    XCount, (XCount & ~(4 << 2))
    .rept   (0x2000 - ((IMX_SDRAM_BASE + IMX_SDRAM_SIZE) >> 29))
    .dword  XCount
    .set    XCount, XCount + 0x20000000
    .endr
#endif

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/board_ttb.S $ $Rev: 960400 $";
.previous
#endif
#endif
