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

/*
** File: usage.c
**
** Description:
**
** This file contains the usage function for the f3s flash file system
**
**
*/

/*
** Includes
*/
#include "f3s_snor.h"

/**
 *  @brief             Use function for F3s flash file system.
 *  @param argc        Argument count.
 *  @param argv        Argument.
 *  @param message     Error message.
 *
 *  @return            None.
 */
void f3s_usage(int argc, char **argv, char *message)
{
    int error = EOK;

    /* check if a message is wanted */

    if (message) {
        error = EINVAL;

        fprintf(stderr, "%s: invalid %s\n", *argv, message);
    }

    exit(error);
}

/*
** End
*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
