/*
 * Copyright (c) 2007, 2022-2023, BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include <string.h>

void *memcpy(void *dst, const void *src, size_t nbytes)
{

    void *const ret = dst;

    /* Both addresses must be aligned to stuff in int size chunks */
    if ((nbytes >= sizeof(unsigned)) &&
        (((uintptr_t)src & (sizeof(unsigned) - 1)) == 0) &&
        (((uintptr_t)dst & (sizeof(unsigned) - 1)) == 0)) {
        unsigned            *d = (unsigned *)dst - 1;
        const unsigned      *s = (const unsigned *)src - 1;

        while (nbytes >= sizeof(unsigned)) {
            nbytes -= sizeof(unsigned);
            *++d = *++s;
        }
        if (nbytes) {
            dst = (unsigned char *)(d + 1);
            src = (const unsigned char *)(s + 1);
        }
    }

    /* Get the unaligned bytes, or the remaining bytes */
    while (nbytes) {
        *(unsigned char *)dst = *(const unsigned char *)src;
        dst = (char *)dst + 1;
        src = (const char *)src + 1;
        --nbytes;
    }

    return ret;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/memcpy.c $ $Rev: 975396 $")
#endif
