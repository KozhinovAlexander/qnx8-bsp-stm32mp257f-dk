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

    .text
    .align  2

    .globl imx_enable_mmu
    .globl imx_disable_mmu

#include "imx_ipl.h"

#if !defined(IMX_SPL_BOOT)
imx_enable_mmu:
    str     x30, [sp,#-16]!

    /* Check if D-Cache is already enabled */
    branch_el_mode x1, 3f, 2f, 1f
3:
    mrs     x0, sctlr_el3
    b       0f
2:
    mrs     x0, sctlr_el2
    b       0f
1:
    mrs     x0, sctlr_el1
0:
    tbnz    x0, #2, done

    /* Set Translate Table Base */
    ldr     x0, =aarch64_arm_board_ttb
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     ttbr0_el3, x0
    b       0f
2:
    msr     ttbr0_el2, x0
    b       0f
1:
    msr     ttbr0_el1, x0
0:

    /* TCR_EL3 Configuration */
    mov     x0, #22             /* 42 bits virtual address */
    orr     x0, x0, #1 << 14    /* AARCH64_TCR_EL3_TG0_64K */
    orr     x0, x0, #1 << 10    /* AARCH64_TCR_EL3_ORGN0_WBWA */
    orr     x0, x0, #1 << 8     /* AARCH64_TCR_EL3_IRGN0_WBWA */
    orr     x0, x0, #3 << 32    /* 42 bits physical address */
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     tcr_el3, x0
    b       0f
2:
    msr     tcr_el2, x0
    b       0f
1:
    msr     tcr_el1, x0
0:

    /*
     * Default MAIR value.
     * Index  MAIR  ARMv8 meaning
     * -----  ----  ------------- -------------
     *     0  0x00  device-nGnRnE strongly-ordered
     *     1  0x04  device-nGnRE  shared-device
     *     2  0x0C  device-GRE    shared-device
     *     3  0x44  normal-NC     normal-NC
     *     4  0xff  normal-NT     normal-NT
     *     5  0x00  reserved      reserved
     *     6  0x00  normal        normal
     *     7  0x00  normal        normal
     */
    ldr     x0, =0x000000FF440C0400
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     mair_el3, x0
    b       0f
2:
    msr     mair_el2, x0
    b       0f
1:
    msr     mair_el1, x0
0:
    isb

    /* Enable MMU, Dcache, Icache */
    branch_el_mode x1, 3f, 2f, 1f
3:
    mrs     x0, sctlr_el3
    b       0f
2:
    mrs     x0, sctlr_el2
    b       0f
1:
    mrs     x0, sctlr_el1
0:
    orr     x0, x0, #(1 << 12)  /* Icache */
    orr     x0, x0, #(1 << 2)   /* Dcache */
    orr     x0, x0, #(1 << 0)   /* MMU */
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     sctlr_el3, x0
    b       0f
2:
    msr     sctlr_el2, x0
    b       0f
1:
    msr     sctlr_el1, x0
0:
    isb

    b       done

imx_disable_mmu:
    str     x30, [sp,#-16]!

    /* Check if MMU is enabled */
    branch_el_mode x1, 3f, 2f, 1f
3:
    mrs     x0, sctlr_el3
    b       0f
2:
    mrs     x0, sctlr_el2
    b       0f
1:
    mrs     x0, sctlr_el1
0:
    tbz     x0, #0, done

    /* Disable Dcache and MMU */
    branch_el_mode x1, 3f, 2f, 1f
3:
    mrs     x0, sctlr_el3
    b       0f
2:
    mrs     x0, sctlr_el2
    b       0f
1:
    mrs     x0, sctlr_el1
    b       0f
0:
    bic     x0, x0, #(1 << 0)   /* MMU */
    bic     x0, x0, #(1 << 2)   /* DCache */
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     sctlr_el3, x0
    b       0f
2:
    msr     sctlr_el2, x0
    b       0f
1:
    msr     sctlr_el1, x0
    b       0f
0:
    isb

    /* Cache Flush */
    bl      aarch64_cache_flush

    /* Invalidate TLB All */
    ic      iallu
    tlbi    vmalle1
    dsb     sy

done:
    ldr     x30, [sp],#16
    ret
#endif

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/board_mmu.S $ $Rev: 960400 $";
.previous
#endif
#endif
