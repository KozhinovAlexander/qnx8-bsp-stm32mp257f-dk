/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2017, 2023, QNX Software Systems.
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
 * This is used to define our ipl memory copy mechanism
*/

#include "ipl.h"
#include <string.h>

#define ALIGNED32(__x)     (((__x) & (paddr_t)0x3) == 0)
#define ALIGNED64(__x)     (((__x) & (paddr_t)0x7) == 0)

static int copy_memory(paddr_t dest, paddr_t src, size_t sz)
{
    short      remainder;

#if defined(__aarch64__)
    // for larger payloads that are aligned use the faster memcpy
    if (ALIGNED64(dest) && ALIGNED64(src) && (sz >= 64)) {
        memcpy_bulk((void *)dest, (const void *)src, sz);
        return (0);
    }

    // src and dest are 64-bits aligned?
    if (ALIGNED64(dest) && ALIGNED64(src)) {
        remainder = (short)(sz & (size_t)0x7);
        sz = sz >> 3;
        while (sz > 0) {
            *(uint64_t *)dest = *(uint64_t *)src;
            dest += 8;
            src += 8;
            sz--;
        }
    }
    else
#endif
    // src and dest are 32-bits aligned?
    if (ALIGNED32(dest) && ALIGNED32(src)) {
        remainder = (short)(sz & (size_t)0x3);
        sz = sz >> 2;
        while (sz > 0) {
            *(uint32_t *)dest = *(uint32_t *)src;
            dest += 4;
            src += 4;
            sz--;
        }
    } else {
        remainder = (short)sz;
    }

    while (remainder > 0) {
        *(uint8_t *)dest = *(uint8_t *)src;
        dest++;
        src++;
        remainder--;
    }

    return (0);
}

/*
 All copying goes through here.  call copy_memory
*/
void copy(const paddr_t dst, const paddr_t src, const size_t size) {
    copy_memory(dst, src, size);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/copy.c $ $Rev: 975396 $")
#endif
