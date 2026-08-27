/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2010, 2011, 2012, 2013, 2014, 2023 BlackBerry Limited.
 * Copyright 2017, 2023 NXP
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
 * Freescale IMX specific STEER interrupt callouts.
 *
 * interrupt_id_* and interrupt_eoi_* are copied and intermixed with other
 * kernel code during initialisation.
 *
 * They do not follow normal calling conventions, and must fall through
 * to the end, rather than attempting to perform a return instruction.
 *
 * The INTR_GENFLAG_* bits in the intrinfo_entry defines which of the
 * following values can be loaded on entry to these code fragments:
 *
 *    x19 - holds the syspageptr                (INTR_GENFLAG_SYSPAGE  set)
 *    x20 - holds the intrinfo_entry pointer    (INTR_GENFLAG_INTRINFO set)
 *    x22 - holds the interrupt mask count      (INTR_GENFLAG_INTRMASK set)
 *
 * The interrupt_id_* routine returns the (controller-relative) level in x19
 *
 * NOTE: The mask register is really a mask register and not an enable. That is
 *       a value of 1 masks and 0 unmasks
 */

#include <aarch64/callout.ah>
#include <soc/nxp/imx8/mp/imx_irqsteer.h>

/*
 * Patch callout code (for STEER)
 *
 * Patcher routine takes the following arguments:
 *  x0 - syspage paddr
 *  x1 - vaddr of callout
 *  x2 - offset from start of syspage to start of callout routine
 *  x3 - offset from start of syspage to rw storage
 *  x4 - patch data
 *  x5 - callout_rtn
 */
interrupt_patch_irqsteer:
    sub     sp, sp, #16
    stp     x19, x30, [sp]
    add     x19, x0, x2             /* x19 = address of callout routine */

    /*
     * Check if we have already mapped registers
     */
    ldr     x0, Lpaddr
    ldr     x1, [x4]                /* x4: patch data = &base */
    cmp     x0, x1
    beq     0f

    /*
     * Map registers
     */
    mov     x0, #0x14               /* Size of interrupt registers */
    bl      callout_io_map
    ldr     x1, =Lvaddr
    str     x0, [x1]
    b       1f
0:
    ldr     x0, Lvaddr
1:
    /*
     * Patch callout with mapped virtual address in x0
     */
    CALLOUT_PATCH   x19, w6, w7

    ldp     x19, x30, [sp]
    add     sp, sp, #16
    ret

    .align  3
Lpaddr:  .quad   -1
Lvaddr:  .quad   -1

/*
 * -----------------------------------------------------------------------
 * Identify STEER interrupt source.
 *
 *  x20 - syspage pointer
 *
 * Returns interrupt number in x19
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_id_imx_irqsteer, 0, interrupt_patch_irqsteer)
    mov     x7, #0xabcd             /* STEER base address (patched) */
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * Read Interrupt Mask and Status
     */
    ldr     w3, [x7, #IMX_IRQ_STEER_MASK]
    ldr     w2, [x7, #IMX_IRQ_STEER_STAT]
    /* Clear any masked bits from the status register */
    ands    w2, w2, w3
    /* set x19 to -1 and return if no bits are set */
    csetm   x19, eq
    beq     done

    clz     w19, w2
    mov     w2, #0x80000000
    lsr     w2, w2, w19

    /* Mask the interrupt source */
    bic      w3, w3, w2
    str      w3, [x7, #IMX_IRQ_STEER_MASK]

    /* Prepare return value */
    mov      w2, #31
    sub      w19, w2, w19

done:
CALLOUT_END(interrupt_id_imx_irqsteer)

/*
 * -----------------------------------------------------------------------
 * Acknowledge specified STEER interrupt
 *
 *  x19 - contains interrupt id
 *  x20 - contains syspage pointer (INTR_GENFLAG_LOAD_SYSPAGE was used)
 *  x22 - contains intr mask count (INTR_GENFLAG_LOAD_INTRMASK was used)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_eoi_imx_irqsteer, 0, interrupt_patch_irqsteer)
    mov     x7, #0xabcd             /* STEER base address (patched) */
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /* Only unmask interrupt if mask count is zero */
    cmp     x22, #0
    bne     0f

    ldr     w1, [x7, #IMX_IRQ_STEER_MASK]
    mov     w2, #0x01
    lsl     w2, w2, w19
    orr     w1, w1, w2
    str     w1, [x7, #IMX_IRQ_STEER_MASK]

0:
CALLOUT_END(interrupt_eoi_imx_irqsteer)


/*
 * -----------------------------------------------------------------------
 * Mask specified STEER interrupt
 *
 *  x0 - _syspage_ptr
 *  x1 - vector number (relative to controller)
 *
 * Returns:
 *    x0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_mask_imx_irqsteer, 0, interrupt_patch_irqsteer)
    mov     x7, #0xabcd             /* STEER base address (patched) */
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * Mask the interrupt
     */
    ldr     w2, [x7, #IMX_IRQ_STEER_MASK]
    mov     w3, #0x01
    lsl     w3, w3, w1
    bic     w2, w2, w3
    str     w2, [x7, #IMX_IRQ_STEER_MASK]

    mov     x0, #0
    ret
CALLOUT_END(interrupt_mask_imx_irqsteer)

/*
 * -----------------------------------------------------------------------
 * Unmask specified STEER interrupt
 *
 *  x0 - _syspage_ptr
 *  x1 - vector number (relative to controller)
 *
 * Returns:
 *    x0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_unmask_imx_irqsteer, 0, interrupt_patch_irqsteer)
    mov     x7, #0xabcd             /* STEER base address (patched) */
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * Unmask the interrupt
     */
    ldr     w2, [x7, #IMX_IRQ_STEER_MASK]
    mov     w3, #0x01
    lsl     w3, w3, w1
    orr     w2, w2, w3
    str     w2, [x7, #IMX_IRQ_STEER_MASK]

    mov     x0, #0
    ret
CALLOUT_END(interrupt_unmask_imx_irqsteer)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/callout_interrupt_steer.S $ $Rev: 984398 $";
.previous
#endif
#endif
