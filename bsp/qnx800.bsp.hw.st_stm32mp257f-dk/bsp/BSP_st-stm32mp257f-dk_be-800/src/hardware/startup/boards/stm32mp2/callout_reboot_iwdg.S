/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
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


/*
 * STM32MP2 system reboot
 */

#include <aarch64/callout.ah>
#include <soc/st/stm32mp2/include/stm32_iwdg.h>
#include "board.h"

/*
 * -----------------------------------------------------------------------
 * Routine to patch callout code
 *
 * On entry:
 *  x0 - physical address of syspage
 *  x1 - virtual  address of syspage
 *  x2 - offset from start of syspage to start of the callout routine
 *  x3 - offset from start of syspage to read/write data used by callout
 * -----------------------------------------------------------------------
 */

patch_reboot:
    sub         sp, sp, #16
    stp         x19, x30, [sp]

    add         x19, x0, x2             /* x19 = address of callout routine */

    /*
     * Map reset control registers
     */
    mov         x0, #0x100
    adr         x1, Lpaddr
    bl          callout_io_map_indirect

    /*
     * Patch the callout routine
     */
    CALLOUT_PATCH   x19, w4, w5

    ldp         x19, x30, [sp]
    add         sp, sp, #16
    ret

    .align 3
Lpaddr: .quad   STM32MP2_IWDG_BASE


CALLOUT_START(reboot_stm32mp2, 0, patch_reboot)
    /* Get the MU base address (patched) */
    mov         x5, #0xabcd
    movk        x5, #0xabcd, lsl #16
    movk        x5, #0xabcd, lsl #32
    movk        x5, #0xabcd, lsl #48

    /* Clear SRS bit to assert ~wdog_rst signal */
    mov         w1, #0x04
    // strh        w1, [x5, #IWDG_KR_RELOAD_KEY]
    // /* Errata TKT039676 - SRS bit must be written twice */
    // strh        w1, [x5, #IWDG_KR_RELOAD_KEY]

0:
    b      0b

CALLOUT_END(reboot_stm32mp2)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/callout_reboot_wdg.S $ $Rev: 984398 $";
.previous
#endif
#endif
