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

#include <string.h>
#include <sys/types.h>

#define WORD_SIZE (SIZEOF32(ulong_t))

// For backward compatibility, the function returns -1, 0 or 1 if the memory
// region pointed to by s1 is less than, equal to or greater than the memory
// region pointed to by s2. While the standard specifies that memcmp() can
// return any negative (respectively, positive) value instead of -1
// (respectively, 1), there may be some client code relying on the previous
// values. This issue may be reexamined in the future, and in general clients
// should not rely on the exact return values.
#define DIFF(u1, u2) (((u1) < (u2)) ? -1 : +1)

static __inline int cmp_unaligned(const unsigned char *c1,
                                  const unsigned char *c2, size_t nbytes)

{
    for (; nbytes > 0; nbytes--) {
        if (*c1 != *c2) {
            return DIFF(*c1, *c2);
        }
        c1++;
        c2++;
    }

    return 0;
}

static __inline int cmp_word(ulong_t u1, ulong_t u2)
{
    // This function is only called if a difference is encountered between two
    // words, so u1 != u2.
#ifdef __LITTLEENDIAN__
    // On a litte-endian processor, the bytes are in reverse order to the
    // memory layout. If we got here, there is a difference between two words,
    // but we have to reverse the byte order to get the correct result.
# if __LONG_BITS__==32
    u1 = __builtin_bswap32(u1);
    u2 = __builtin_bswap32(u2);
# elif __LONG_BITS__==64
    u1 = (ulong_t)__builtin_bswap64(u1);
    u2 = (ulong_t)__builtin_bswap64(u2);
# else
#   error if-chain needs updating
# endif
#endif
    return DIFF(u1, u2);
}

// Let the compiler optimize this function.
int memcmp(const void *__s1, const void *__s2, size_t __n)
    __attribute__((__pure__, __hot__, __optimize__(3, "unroll-loops")));


int memcmp(const void *const __s1, const void *const __s2, size_t __n)
{

    const unsigned char *c1 = __s1;
    const unsigned char *c2 = __s2;
    unsigned align;

    // Check if the pointers are similarly-aligned.
    align = (unsigned)((paddr_t)c1) & (WORD_SIZE - 1);
    if (align != ((unsigned)((paddr_t)c2) & (WORD_SIZE - 1))) {
        // No, do byte-wise comparison.
        return cmp_unaligned(c1, c2, __n);
    }

    // Check if both pointers are word aligned.
    if (align) {
        // No, compare until word aligned.
        for (; (align < WORD_SIZE) && (__n > 0); __n--) {
            if (*c1 != *c2) {
                return DIFF(*c1, *c2);
            }
            align++;
            c1++;
            c2++;
        }
    }

    // Compare in chunks of one word.
    for (; __n >= WORD_SIZE; __n -= WORD_SIZE) {
        if (*(ulong_t *)c1 != *(ulong_t *)c2) {
            return cmp_word(*(ulong_t *)c1, *(ulong_t *)c2);
        }
        c1 += WORD_SIZE;
        c2 += WORD_SIZE;
    }

    // Compare remaining bytes, if any.
    return cmp_unaligned(c1, c2, __n);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/memcmp.c $ $Rev: 975396 $")
#endif
