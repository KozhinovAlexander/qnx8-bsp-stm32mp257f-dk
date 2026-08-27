/*
 * Copyright (c) 2022, BlackBerry Limited.
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

#include <stdarg.h>
#include "f3s_snor.h"

/**
 *  @brief             Logging function.
 *  @param severity    Logging severity.
 *  @param verbosity   Driver verbosity.
 *  @param vlevel      Verbosity level.
 *  @param fmt         String that contains the text to be logged.
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_slogf(const int severity, const int verbosity, const int vlevel, const char* const fmt, ...)
{
    ssize_t     ret;
    va_list     arglist;

    ret = 0;

    if (verbosity > 5) {
        va_start(arglist, fmt);
        vfprintf(stderr, fmt, arglist);
        va_end(arglist);
        fprintf(stderr, "\n");
    }

    if (verbosity >= vlevel) {
        va_start(arglist, fmt);
        ret = vslogf(_SLOGC_FS_FFS, severity, fmt, arglist);
        va_end(arglist);
    }

    return (ret);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
