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
** File: snor_alloc_handle.c
**
** Description:
**
** This file contains the device handle allocation
**
** Ident: $Id: snor_alloc_handle.c $
*/

/*
** Includes
*/

#include "f3s_snor.h"

/**
 *  @brief             Allocate driver handle.
 *  @param socket      Pointer to flash socket structure.
 *  @param nbytes      Driver handle structure size in bytes.
 *
 *  @return            Pointer to driver handle --success otherwise(NULL) fail.
 */
void *snor_alloc_handle(f3s_socket_t* const socket, const size_t nbytes)
{
    snor_ctrl_t *ctrl;

    if (socket->memory != NULL) return (socket->memory);

    /* Allocate driver handle */
    ctrl = calloc(1, nbytes);
    if (ctrl == NULL) {
        snor_slogf(_SLOG_ERROR, 0, 0, "%s: Could not allocate memory", __func__);
        return (NULL);
    }

    ctrl->ncs   = 1;        /* default to one chip select */

    for (int i = 0; i < SNOR_MAX_CS; i++) {
        ctrl->chip[i].hcmask        = 0xFFFFFFFF;   /* hardware capability mask */
        ctrl->chip[i].dcmask        = 0xFFFFFFFF;   /* flash device capability mask */
        ctrl->chip[i].align         = 1;
        ctrl->chip[i].cfg.bus_proto = SNOR_BUSPROTO_1_1_1;
        ctrl->chip[i].ctrl          = ctrl;
        ctrl->chip[i].post_ident    = snor_post_ident;
        ctrl->chip[i].lock_v        = 1;            /* Dynamic lock value */
        ctrl->chip[i].lock_m        = 1;            /* Dynamic lock mask */
    }

    socket->memory      = (uint8_t *)ctrl;
    socket->address     = 0x534E4F52;               /* SNOR, not important */
    socket->window_size = 0x10000;                  /* The size isn't important */
    socket->array_size  = 0x10000;                  /* The size isn't important */

    if (socket->option) {
        if (f3s_socket_option(socket) != EOK) {
            free((void *)ctrl);
            return (NULL);
        }
    }

    return (socket->memory);
}

/*
** End
*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
