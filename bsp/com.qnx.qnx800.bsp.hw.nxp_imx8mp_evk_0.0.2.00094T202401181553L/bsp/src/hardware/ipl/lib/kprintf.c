/*
 * Copyright (c) 2019, 2022-2023, BlackBerry Limited.
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


#include "ipl.h"
#include <stdarg.h>

static const char c[] = "0123456789abcdef";

static void
vmsg(const char *fmt, const va_list args) {
    char            *vs;
    uint64_t        num;
    unsigned        radix;
    int             dig;
    char            buf[64];

    //for(; *fmt; ++fmt) {
    while (*fmt) {
        if(*fmt != '%') {
            ser_putchar(*fmt);
            fmt++;
            continue;
        }
        radix = 0;
        dig = 0;
        switch(*++fmt) {
        case 'b':
            num = va_arg(args, unsigned);
            dig = (int)sizeof(uint8_t)*2;
            radix = 16;
            break;
        case 'w':
            num = va_arg(args, unsigned);
            dig = (int)sizeof(uint16_t)*2;
            radix = 16;
            break;
        case 'P':
            num = va_arg(args, paddr_t);
            dig = (int)sizeof(paddr_t)*2;
            radix = 16;
            break;
        case 'x':
            num = va_arg(args, unsigned);
            dig = (int)sizeof(unsigned)*2;
            radix = 16;
            break;
        case 'l':
            num = va_arg(args, unsigned long);
            dig = (int)sizeof(unsigned long)*2;
            radix = 16;
            break;
        case 'L':
            num = va_arg(args, uint64_t);
            dig = (int)sizeof(uint64_t)*2;
            radix = 16;
            break;
        case 'v':/* abbreviated hex */
            num = va_arg(args, uintptr_t);
            dig = (int)sizeof(uintptr_t)*2;
            radix = 16;
            break;
        case 'd':
        case 'u':
            num = va_arg(args, unsigned);
            radix = 10;
            break;
        case 's':
            vs = va_arg(args, char *);
            while(*vs) {
                ser_putchar(*vs++);
            }
            fmt++;
            continue;
        default:
            ser_putchar(*fmt);
            fmt++;
            continue;
        }
        vs = &buf[sizeof(buf)];

        *--vs = '\0';
#if 0
        do {
            *--vs = c[num % radix];
            num /= radix;
        } while(num);
#else
        if (radix == 10) {
            do {
                *--vs = c[(unsigned)num % radix];
                num = (unsigned)num / radix;
            } while(num);
        } else {    // radix == 16
            do {
                *--vs = c[num & 0xFULL];
                num >>= 4;
            } while(num);
        }
#endif
        for(dig -= &buf[sizeof(buf)-1] - vs; dig > 0; --dig) {
            ser_putchar('0');
        }
        while(*vs) {
            ser_putchar(*vs++);
        }
        fmt++;
    }
}

void
kprintf(const char *const fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vmsg(fmt, args);
    va_end(args);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/kprintf.c $ $Rev: 975396 $")
#endif
