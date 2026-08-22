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
** File: f3s_flash_list.c
**
** Description:
**
** This file contains the flash list function for the f3s flash file system
**
** Ident: $Id: flash_list.c 680332 2012-11-27 01:28:14Z builder@qnx.com $
**
*/

/*
** Includes
*/

#include "f3s_snor.h"

/**
 *  @brief             Flash list callout
 *                     SPI NOR flash doesn't support this structure.
 *  @param flash       Pointer to flash structure
 *
 *  @return            None
 */
void f3s_flash_list(f3s_flash_t *flash)
{
    /* We don't support this */
    exit(ENOTSUP);
}

/*
** End
*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
