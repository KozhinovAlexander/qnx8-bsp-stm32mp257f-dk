#
# Copyright 2016, 2022 BlackBerry Limited.
# Copyright 2016, Freescale Semiconductor, Inc.
# Copyright 2017 NXP
#
# Licensed under the Apache License, Version 2.0 (the "License"). You
# may not reproduce, modify or distribute this software except in
# compliance with the License. You may obtain a copy of the License
# at: http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as
# contributors under the License or as licensors under other terms.
# Please review this entire file for other proprietary rights or license
# notices, as well as the QNX Development Suite License Guide at
# http://licensing.qnx.com/license-guide/ for other information.
#

/*
 * _start.S
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 *
 * For the ARMv8 Fast Model, all processors execute from cold reset in EL3.
 * This code sets up minimal EL2/EL3 state for all cpus and switches to EL1
 * to continue execution:
 * - cpu0 jumps to cstart to perform the startup initialisation
 * - other cpus spin until they are woken up by board_smp_start
 */

#include "board.h"

    .text
    .align 2

    .extern cstart
    .global _start

_start:
    /* NOTE:
     *  Do NOT modify registers X0-X3 before jumping to the cstart label.
     *  These registers may contain information provided by the IPL and
     *  cstart will save them in the boot_regs variable for later perusal
     *  by other portions of startup.
     */
    /*
     * IPL/u-boot starts all cores at the ELF entry point.
     * Secondary cores wait until the boot cpu wakes them up.
     *
     * FIXME_AARCH64: this will need work to handle multiple clusters.
     */
    mrs         x9, mpidr_el1
    and         x9, x9, #0xFFFF             /* Mask Affinity 1:0 */
    cbnz        x9, secondary_core          /* Is (Affinity 1:0) > 1 ? secondary_core */

    /*
     * Set up EL3 state if necessary
     */
    mrs         x9, CurrentEL
    cmp         x9, #0xC                    /* Is core in EL3 mode? */
    bne         master_core
    bl          setup_el3

master_core:
    b           cstart

    /*
     * Increment the number of processors so board_smp_num_cpus()
     * knows how many processors are present.
     * This assumes all secondary processors get through this code
     * before board_smp_num_cpus() executes on the boot processor.
     */
secondary_core:
    /*
     * Wait until board_smp_start sets secondary_cpu to our core number.
     */
    adr         x1, secondary_start
    adr         x2, secondary_cpu
000:
    wfe
    dmb         sy
    ldr         x3, [x2]
    mrs         x0, mpidr_el1
    and         x0, x0, #0xFFFF             /* Aff1:0 */
    cmp         x3, x0
    bne         000b
    ldr         x3, [x1]
    br          x3

setup_el3:
    /*
     * Set default generic timer frequency, if not set already
     */
    mrs         x4, cntfrq_el0
    cbnz        x4, 001f
    ldr         w4, cntfrq_default
    msr         cntfrq_el0, x4
001:
    ret

cntfrq_default:
    .word IMX_CNTV_CLOCK_FREQ   /* Default cntfrq_el0 value */

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/_start.S $ $Rev: 957658 $";
.previous
#endif
#endif
