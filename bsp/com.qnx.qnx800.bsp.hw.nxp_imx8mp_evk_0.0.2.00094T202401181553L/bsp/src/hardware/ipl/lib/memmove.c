/*
 * Copyright (c) 2008, 2023, BlackBerry Limited.
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

void *
memmove(void *const __s1, const void *const __s2, size_t __n) {
    char        *const d = __s1;
    const char  *const s = __s2;

    if ((s < d) && ((s + __n) > d)) {
		/* pointers overlapping, have to copy backwards */
		for( ;; ) {
            --__n;
            if (__n == (size_t)-1) break;
            d[__n] = s[__n];
		}
	} else {
	/*
     *	use optimized memcpy (duff's device based)
         */
        memcpy(__s1, __s2, __n);
	
	}
    return (__s1);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/memmove.c $ $Rev: 975396 $")
#endif
