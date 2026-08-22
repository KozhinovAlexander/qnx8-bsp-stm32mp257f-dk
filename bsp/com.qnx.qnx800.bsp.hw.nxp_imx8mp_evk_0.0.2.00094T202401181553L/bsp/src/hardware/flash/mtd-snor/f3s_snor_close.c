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

#include "f3s_snor.h"


/**
 *  @brief             Close callout for SPI serial NOR flash.
 *  @param socket      Pointer to flash socket structure.
 *  @param flags       Close flags
 *
 *  @return            None
 */
void f3s_snor_close(f3s_socket_t *socket, uint32_t flags)
{
    snor_ctrl_t* const ctrl = (snor_ctrl_t *)socket->memory;

    if (ctrl->funcs.dinit != NULL) {
        ctrl->funcs.dinit(ctrl);
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
