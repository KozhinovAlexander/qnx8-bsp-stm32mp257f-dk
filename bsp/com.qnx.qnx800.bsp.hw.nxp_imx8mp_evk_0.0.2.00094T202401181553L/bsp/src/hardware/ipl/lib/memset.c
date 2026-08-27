/*
 * Copyright (c) 2016, 2023, BlackBerry Limited.
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

#include <inttypes.h>
#include <sys/types.h>
#include "ipl.h"

void *memset(void *const str, const int value, size_t num) {
    volatile unsigned char *ptr = str;

    /* Stuff unaligned addresses first */
    while (((uintptr_t)ptr & (sizeof(unsigned) - 1)) && num) {
        *ptr++ = (unsigned char)value;
        num--;
    }

    /* Now stuff in native int size chunks if we can */
    if (num >= sizeof(unsigned)) {
#if __INT_BITS__ == 32
        const unsigned cc = 0x01010101U * (unsigned char)value;
#elif __INT_BITS__ == 64
        const unsigned cc = 0x0101010101010101ULL * (unsigned char)value;
#else
#error Unknown __INT_BITS__ size
#endif
        unsigned *pp = (unsigned *)ptr - 1;

        while (num >= sizeof(unsigned)) {
            num -= sizeof(unsigned);
            *++pp = cc;
        }
        if (num) {
            ptr = (unsigned char *)(pp + 1);
        }
    }

    /* Get the remaining bytes */
    if (num) {
        ptr--;
        while (num) {
            num--;
            *++ptr = (unsigned char)value;
        }
    }

    return str;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/memset.c $ $Rev: 980326 $")
#endif
