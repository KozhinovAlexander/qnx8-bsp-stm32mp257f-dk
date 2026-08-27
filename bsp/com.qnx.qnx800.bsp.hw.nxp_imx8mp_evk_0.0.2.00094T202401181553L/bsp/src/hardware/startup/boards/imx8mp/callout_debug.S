/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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
 * i.MX UART specific polled serial I/O
 */

#include <aarch64/callout.ah>
#include "board.h"
#include <soc/nxp/imx8/common/imx_uart.h>

/*
 * Patch interrupt callouts to access rw data.
 * The first call will also map the uart.
 *
 * Patcher routine takes the following arguments:
 *    x0 - syspage paddr
 *    x1 - vaddr of callout
 *    x2 - offset from start of syspage to start of callout routine
 *    x3 - offset from start of syspage to rw storage
 *    x4 - patch data
 *    x5 - callout_rtn
 */

patch_debug:
    sub        sp, sp, #16
    stp        x19, x30, [sp]

    add        x19, x0, x2        // x19 = address of callout routine

    /* Map UART using patch_data parameter */
    mov        x0, #IMX_UART_SIZE
    ldr        x1, [x4]
    bl         callout_io_map

    /* Patch callout with mapped virtual address in x0 */
    CALLOUT_PATCH    x19, w6, w7

    ldp        x19, x30, [sp]
    add        sp, sp, #16
    ret

/*
 * -----------------------------------------------------------------------
 * void    imx_uart_display_char(struct sypage_entry *, char)
 *
 * x0: syspage pointer
 * x1: character
 * -----------------------------------------------------------------------
 */
CALLOUT_START(imx_uart_display_char, 0, patch_debug)
    mov        x7, #0xabcd                // UART base address (patched)
    movk       x7, #0xabcd, lsl #16
    movk       x7, #0xabcd, lsl #32
    movk       x7, #0xabcd, lsl #48

0:
    ldr        w2, [x7, #IMX_UART_USR1]
    tst        w2, #IMX_UART_USR1_TRDY
    beq        0b
    and        w0, w1, #0xff
    str        w0, [x7, #IMX_UART_UTXD]
    ret
CALLOUT_END(imx_uart_display_char)


/*
 * -----------------------------------------------------------------------
 * char    imx_uart_poll_key(struct sypage_entry *)
 *
 * x0: syspage pointer
 * -----------------------------------------------------------------------
 */
CALLOUT_START(imx_uart_poll_key, 0, patch_debug)
    mov     x7, #0xabcd                // UART base address (patched)
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48


    ldr     w1, [x7, #IMX_UART_USR2]
    tst     w1, #IMX_UART_USR2_RDR
    beq     0f
    ldr     w0, [x7, #IMX_UART_URXD]
    ret
0:
    mov     w0, #-1
    ret
CALLOUT_END(imx_uart_poll_key)


/*
 * -----------------------------------------------------------------------
 * int break_detect(struct syspage_entry *)
 *
 * x0: syspage pointer
 *
 * Return 1 if break detected, 0 otherwise
 * -----------------------------------------------------------------------
 */
CALLOUT_START(imx_uart_break_detect, 0, patch_debug)
    mov     x7, #0xabcd                // UART base address (patched)
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * FIXME: need to implement
     */
     /*
    ldr        r0, [ip, #IMX_UART_USR2]
    ands       r0, r0, #IMX_UART_USR2_BRCD
    beq        0f
    str        r0, [ip, #IMX_UART_USR2]
    ldr        r0, [ip, #IMX_UART_URXD]
    mov        r0, #1
    */

    mov        w0, #0
0:
    ret
CALLOUT_END(imx_uart_break_detect)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/callout_debug.S $ $Rev: 984398 $";
.previous
#endif
#endif
