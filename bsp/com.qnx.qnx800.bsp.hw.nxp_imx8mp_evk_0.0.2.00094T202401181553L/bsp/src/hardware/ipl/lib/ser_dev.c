/*
 * Copyright (c) 2007, 2008, 2022-2023, BlackBerry Limited.
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


#include <stddef.h>
#include "ipl.h"
#include <hw/inout.h>

static ser_dev *ser = NULL;

void init_serdev(ser_dev *const dev)
{
    ser = dev;
}

char ser_poll(void)
{
    if (ser == 0) return '\0';
    return (ser->poll());
}

char ser_getchar(void)
{
    if (ser == 0) return '\0';
    return (ser->get_byte());
}

void ser_putchar(const char chr)
{
    if (ser == 0) return;
    if (chr == '\n') {
        ser->put_byte('\r');
    }
    ser->put_byte(chr);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/ser_dev.c $ $Rev: 975396 $")
#endif
