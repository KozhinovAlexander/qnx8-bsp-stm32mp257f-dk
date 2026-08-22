/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
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
 * $
 */

#include <aarch64/gic_v3.h>
#include "board.h"
#include "imx_ipl.h"

    .global _start
    .global __ipl_stack_top
    .global wake_secondary_core

_start:
    adr     x0, vectors
    branch_el_mode x1, 3f, 2f, 1f
3:
    msr     VBAR_EL3, x0            /* EL3 configuration */
    mrs     x1, SCR_EL3
    mov     x2, #((1<<10)|(1<<8)|(1<<0))
    orr     x1, x1, x2              /* SCR_EL3.RW, HCE, NS set */
    bic     x1, x1, #0x0E           /* IRQ & FIQ & EA set to 0 */
    msr     SCR_EL3, x1             /* Set EL2/EL1/EL0 to AArch64 */
    msr     CPTR_EL3, xzr           /* Enable FP/SIMD */

    /* Disable MMU and Caches */
    mrs     x0, SCTLR_EL3
    bic     x0, x0, #(0x01 << 0)    /* Clear bit M(0) - EL3 stage 1 address translation disabled. */
    orr     x0, x0, #(0x01 << 1)    /* Set bit A(1)   - Alignment fault checking enabled. */
    bic     x0, x0, #(0x01 << 2)    /* Disable D Cache */
    bic     x0, x0, #(0x01 << 12)   /* Disable I Cache */
    msr     SCTLR_EL3, x0

    /* Initialize CNTFRQ */
    mov     x0, #(IMX_COUNTER_FREQUENCY & 0xFFFF)
    movk    x0, #(IMX_COUNTER_FREQUENCY >> 16), lsl #16
    msr     cntfrq_el0, x0
    b       0f
2:
    msr     VBAR_EL2, x0            /* EL2 configuration */
    mov     x1, #0x33FF
    msr     CPTR_EL2, x1

    /* Disable MMU and Caches */
    mrs     x0, SCTLR_EL2
    bic     x0, x0, #(0x01 << 0)    /* Clear bit M(0) - EL2 stage 1 address translation disabled. */
    orr     x0, x0, #(0x01 << 1)    /* Set bit A(1)   - Alignment fault checking enabled. */
    bic     x0, x0, #(0x01 << 2)    /* Disable D Cache */
    bic     x0, x0, #(0x01 << 12)   /* Disable I Cache */
    msr     SCTLR_EL2, x0
    b       0f
1:
    msr     VBAR_EL1, x0            /* EL1 configuration */
    mov     x0, #0x300000
    msr     CPACR_EL1, x0

    /* Disable MMU and Caches */
    mrs     x0, SCTLR_EL1
    bic     x0, x0, #(0x01 << 0)    /* Clear bit M(0) - EL2 stage 1 address translation disabled. */
    orr     x0, x0, #(0x01 << 1)    /* Set bit A(1)   - Alignment fault checking enabled. */
    bic     x0, x0, #(0x01 << 2)    /* Disable D Cache */
    bic     x0, x0, #(0x01 << 12)   /* Disable I Cache */
    msr     SCTLR_EL1, x0
0:
    dsb     sy
    isb
    ic      ialluis
    isb     sy
    mov     x0, x0
    mov     x0, x0
    mov     x0, x0

    msr     DAIFset, #0x0F          /* Disable interrupts etc. */

    mrs     x0, MPIDR_EL1
    and     x0, x0, #0xFFFF         /* Cluster - Aff0,1 > 0 ? jmp secondary_core : jmp primary_core*/
    cbnz    x0, secondary_core

primary_core:

    branch_el_mode x1, 3f, 2f, 1f
3:
    /* Enable I Cache in EL3 */
    mrs     x0, SCTLR_EL3
    orr     x0, x0, #(1 << 12)      /* Set bit I[12] */
    msr     SCTLR_EL3, x0
    b       0f
2:
    /* Enable I Cache in EL2 */
    mrs     x0, SCTLR_EL2
    orr     x0, x0, #(1 << 12)      /* Set bit I[12] */
    msr     SCTLR_EL2, x0
    b       0f
1:
    /* Enable I Cache in EL1 */
    mrs     x0, SCTLR_EL1
    orr     x0, x0, #(1 << 12)      /* Set bit I[12] */
    msr     SCTLR_EL1, x0
0:
    dsb     sy
    isb
    ic      ialluis
    isb     sy
    mov     x0, x0
    mov     x0, x0
    mov     x0, x0

    /* Setup the Stack */
    ldr     x0, stack_top
    mov     sp, x0

    bl      _main

oops:
    b       oops

secondary_core:

slave_cpu:
    wfe
    adr     x0, secondary_core_jump_address
    ldr     x1, [x0]
    cbz     x1, slave_cpu
    br      x1

wake_secondary_core:
    adr     x1, secondary_core_jump_address
    str     x0, [x1]
    sev
    ret

    .align  4
secondary_core_jump_address:
    .quad   0x00


    .align  4
stack_top:
    .long   __ipl_stack_top
    .long   0

init_core_gic_v3:
    /* Initialize Cpu Interface */
    branch_el_mode x1, 3f, 2f, 1f
3:
    mrs     x10, ARM_GICC_SRE_EL3
    orr     x10, x10, #0x0F         /* 0x0F SRE & Disable IRQ/FIQ Bypass & Allow EL2 access to ICC_SRE_EL2 */
    msr     ARM_GICC_SRE_EL3, x10
    isb
2:
    mrs     x10, ARM_GICC_SRE_EL2
    orr     x10, x10, #0x0F         /* 0x0F SRE & Disable IRQ/FIQ Bypass & Allow EL1 access to ICC_SRE_EL1 */
    msr     ARM_GICC_SRE_EL2, x10
    isb
1:
    mrs     x10, ARM_GICC_SRE_EL1
    orr     x10, x10, #0x0F         /* 0x0F SRE & Disable IRQ/FIQ Bypass & Allow EL1 access to ICC_SRE_EL1 */
    msr     ARM_GICC_SRE_EL1, x10
    isb

    /* Initialize CPU Redistributor */
    ldr     x1, =IMX_GIC_GICR_BASE
    mrs     x10, MPIDR_EL1
    lsr     x9, x10, #32
    bfi     x10, x9, #24, #8        /* w10 is aff3:aff2:aff1:aff0 */
    mov     x9, x1
2:
    ldr     x11, [x9, ARM_GICR_TYPER]
    lsr     x11, x11, #32           /* w11 is aff3:aff2:aff1:aff0 */
    cmp     w10, w11
    b.eq    3f
    add     x9, x9, #(2 << 16)
    b       2b
3:
    mov     w10, #~0x2
    ldr     w11, [x9, ARM_GICR_WAKER]
    and     w11, w11, w10                   /* Clear ProcessorSleep */
    str     w11, [x9, ARM_GICR_WAKER]
    dsb     st
    isb
4:
    ldr     w10, [x9, ARM_GICR_WAKER]
    tbnz    w10, #2, 4b                     /* Wait Children be Alive */

    add     x10, x9, #(1 << 16)             /* SGI_Base */
    mov     w11, #0xFFFFFFFF
    str     w11, [x10, ARM_GICR_IGROUPR0]
    str     wzr, [x10, ARM_GICR_IGRPMODR0]  /* SGIs|PPIs Group1NS */
    mov     w11, #0x01                      /* Enable SGI 0 */
    str     w11, [x10, ARM_GICR_ISENABLER0]

    ret

/*
 * Exception vectors table
 */
    .align    11
    .globl    vectors
vectors:
bad_sync_hndl:
    b         bad_sync_hndl   /* Synchronous Thread */
    .align    7
bad_irq_hndl:
    b         bad_irq_hndl    /* IRQ Thread */

    .align    7
bad_fiq_hndl:
    b         bad_fiq_hndl    /* FIQ Thread */

    .align    7
bad_error_hndl:
    b         bad_error_hndl  /* Error Thread */

    .align    7
sync_hndl:
    b         sync_hndl       /* Synchronous Handler */

    .align    7
irq_hndl:
    b         irq_hndl        /* IRQ Handler */

    .align    7
fiq_hndl:
    b         fiq_hndl        /* FIQ Handler */

    .align    7
error_hndl:
    b         error_hndl      /* Error Handler */

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/_start.S $ $Rev: 980064 $";
.previous
#endif
#endif
