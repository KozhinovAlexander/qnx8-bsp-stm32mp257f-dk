/*
 * Copyright (c) 2017, 2022, BlackBerry Limited.
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
 * memcpy_bulk.S
 *
 * An AArch64 assembler optimized memcpy tailored for large bulk payload transfers.
 *
 * Exported functions:
 *      void memcpy_bulk( void *dst, const void *src, size_t size )
 *
 * Usage requirements: Both src and dst must be 8-byte aligned if aligned memory
 *                     checks are enabled in SCTLR_ELx.A
 *
 */

    .text
    .align 2

/*
 * void memcpy_bulk( void *dst, const void *src, size_t size )
 */
memcpy_bulk:
    sub sp, sp, #32
    stp x19, x20, [sp]
    stp x21, x22, [sp, #16]

    /* determine how many whole 128-byte sized chunks to copy: (size / 128) */
    lsr x22, x2, #7

    /* if the size to copy is less than 128-bytes then copy the next largest chunk 64, 32, 16, ... */
    cmp x22, #0
    beq 1f

128:
    /* load prefetch memory hint */
    prfm PLDL1STRM, [x1]
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    ldp x7, x8, [x1, #32]
    ldp x9, x10, [x1, #48]
    ldp x11, x12, [x1, #64]
    ldp x13, x14, [x1, #80]
    ldp x15, x19, [x1, #96]
    ldp x20, x21, [x1, #112]

    /* store prefetch memory hint */
    prfm PSTL1STRM, [x0]
    stp x3, x4, [x0]
    stp x5, x6, [x0, #16]
    stp x7, x8, [x0, #32]
    stp x9, x10, [x0, #48]
    stp x11, x12, [x0, #64]
    stp x13, x14, [x0, #80]
    stp x15, x19, [x0, #96]
    stp x20, x21, [x0, #112]

    add x0, x0, #128
    add x1, x1, #128
    subs x22, x22, #1
    bne 128b

1:
    /* clear all the upper bits */
    ands x2, x2, #0x7F
2:
    /* if there are no bytes left to copy return */
    beq 0f

    /* check if there are at least 64, 32, 16, 8 or 4 bytes left to copy */
    clz w22, w2
    cmp w22, #25
    beq 64f
    cmp w22, #26
    beq 32f
    cmp w22, #27
    beq 16f
    cmp w22, #28
    beq 8f
    cmp w22, #29
    beq 4f

    /* if there are 3-bytes or less left then copy them byte-by-byte */
    lsl w2, w2, #3
    adr x3, 0f
    sub x3, x3, x2
    /* jump to one of the three forward lables: 3, 2, 1 */
    br  x3

64:
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]
    ldp x7, x8, [x1, #32]
    ldp x9, x10, [x1, #48]

    stp x3, x4, [x0]
    stp x5, x6, [x0, #16]
    stp x7, x8, [x0, #32]
    stp x9, x10, [x0, #48]

    add x0, x0, #64
    add x1, x1, #64
    ands x2, x2, #0x3F
    b   2b

32:
    ldp x3, x4, [x1]
    ldp x5, x6, [x1, #16]

    stp x3, x4, [x0]
    stp x5, x6, [x0, #16]

    add x0, x0, #32
    add x1, x1, #32
    ands x2, x2, #0x1F
    b   2b

16:
    ldp x3, x4, [x1], 16
    stp x3, x4, [x0], 16

    ands x2, x2, #0xF
    b   2b

8:
    ldr x3, [x1], 8
    str x3, [x0], 8

    ands x2, x2, #0x7
    b   2b

4:
    ldr w3, [x1], 4
    str w3, [x0], 4

    ands x2, x2, #0x3
    b   2b

3:
    ldrb w3, [x1], #1
    strb w3, [x0], #1
2:
    ldrb w3, [x1], #1
    strb w3, [x0], #1
1:
    ldrb w3, [x1]
    strb w3, [x0]

0:
    ldp x19, x20, [sp]
    ldp x21, x22, [sp, #16]
    add sp, sp, #32
    ret

    .global memcpy_bulk
    .type memcpy_bulk, function
    .size memcpy_bulk, . - memcpy_bulk

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/memcpy_bulk.S $ $Rev: 953052 $";
.previous
#endif
#endif
