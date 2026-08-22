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
 *  @brief             Page callout for SPI serial NOR flash.
 *  @param socket      Pointer to socket structure.
 *  @param flags       Page flags.
 *  @param offset      Page offset.
 *  @param size        Pointer to size limit.
 *
 *  @return            Pointer to chip structure --success otherwise NULL.
 */
uint8_t *f3s_snor_page(f3s_socket_t *socket, uint32_t flags, uint32_t offset, int32_t *size)
{
    snor_ctrl_t *ctrl = (snor_ctrl_t *)socket->memory;
    snor_chip_t *chip;
    uint8_t     cs;
    uint32_t    top = 0;

    // we use chip->offset for now, or we can use socket->window_offset
    // socket->window_offset = 0;

    // offset is chip select, for flash probe
    if (flags == SNOR_FLG_FORCECS) {
        return (uint8_t *)&ctrl->chip[offset];
    }

    chip = &ctrl->chip[0];

    // Stripe mode?
    if (ctrl->flags & SNOR_FLG_STRIPE) {
        ctrl->ccs = 0;
        chip->offset = offset / chip->cfg.ncs;
        if (size) {
            // ensure that offset + size is not out of bounds
            *size = min(*size, (int32_t)(chip->chipsz * (uint32_t)chip->cfg.ncs - offset));
        }
        return (uint8_t *)chip;
    }

    uint32_t chip_offset = offset;
    for (cs = 0; cs < ctrl->ncs; cs++) {
        top += chip->chipsz;
        if (top > offset) {
            ctrl->ccs = cs;
            if (size) {
                // ensure that offset + size is not out of bounds
                *size = min(*size, (int32_t)(top - offset));
            }
            chip->offset = chip_offset;
            return (uint8_t *)chip;
        }
        chip_offset -= chip->chipsz;
        chip++;
    }

    return (NULL);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
