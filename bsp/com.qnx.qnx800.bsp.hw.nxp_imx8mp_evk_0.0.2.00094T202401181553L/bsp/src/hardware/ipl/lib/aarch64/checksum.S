/*
 * Copyright (c) 2013, 2022, BlackBerry Limited.
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
 * $
 */

    .text
    .align    2
    .globl    checksum
    .type     checksum, function

/*
 * unsigned checksum(paddr_t addr, size_t len);
 * mkifs generated startup and IFS are aligned to word address
 * Also the length is integer multiple of word size
 */
checksum:
    stp     x2, x3, [sp,#-16]!  /* Save x2-x5 registers */
    stp     x4, x5, [sp,#-16]!

    and     x2, x1, #~(0x001F)
    add     x2, x2, x0
    mov     x3, #0              /* Initial checksum value */


0:                              /* Calculate checksum in 32B blocks */
    ldp     w4, w5, [x0, #0]
    add     w3, w3, w4
    add     w3, w3, w5
    ldp     w4, w5, [x0, #8]
    add     w3, w3, w4
    add     w3, w3, w5
    ldp     w4, w5, [x0, #16]
    add     w3, w3, w4
    add     w3, w3, w5
    ldp     w4, w5, [x0, #24]
    add     w3, w3, w4
    add     w3, w3, w5
    add     x0, x0, #32         /* Increment data load index */
    cmp     x0, x2
    b.ne    0b

    and     x2, x1, #0x001F     /* Calculate checksum remained */
    add     x2, x2, x0          /* End of the source address */
    cmp     x0, x2
    b.eq    2f                  /* Is check area size divided by 32B? */

1:                              /* Calculate checksum remained data area (< 32B) */
    ldr     w4, [x0, #0]
    add     w3, w3, w4
    add     x0, x0, #4
    cmp     x0, x2
    b.ne    1b

2:
    mov     w0, w3              /* Return checksum value */

    ldp     x4, x5, [sp],#16    /* Restore x2-x5 registers */
    ldp     x2, x3, [sp],#16
    ret

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/checksum.S $ $Rev: 953052 $";
.previous
#endif
#endif
