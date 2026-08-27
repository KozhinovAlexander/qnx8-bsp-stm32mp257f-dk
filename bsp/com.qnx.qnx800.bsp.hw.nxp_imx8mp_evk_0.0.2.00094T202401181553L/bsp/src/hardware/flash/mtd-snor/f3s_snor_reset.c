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

static void _snor_reset_chip(snor_chip_t *chip);

/**
 *  @brief             Reset callout for SPI serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Reset  flags.
 *  @param offset      Reset offset.
 *
 *  @return            None
 */
void f3s_snor_reset(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;

    chip = (snor_chip_t *)access->service->page(&access->socket, flags, offset, NULL);
    if (chip == NULL) return;

    if (snor_reset(chip) == EOK) {
        ctrl = chip->ctrl;
        if (ctrl->flags & SNOR_FLG_STRIPE) {
            ctrl->flags &= ~SNOR_FLG_STRIPE;
            const uint8_t csf = ctrl->chip[0].cfg.cs;
            uint8_t ncs, cs;
            for (cs = 0; cs < SNOR_MAX_CS; cs++) {
                chip = &ctrl->chip[cs];
                if (chip->flags & SNOR_CFLG_PRESENT) {
                    ncs = chip->cfg.ncs;
                    chip->cfg.ncs = 1;
                    chip->cfg.cs = cs;
                    _snor_reset_chip(chip);
                    chip->cfg.ncs = ncs;
                }
            }
            ctrl->chip[0].cfg.cs   = csf;
            ctrl->chip[0].rdcfg.cs = csf;
            ctrl->chip[0].wrcfg.cs = csf;
            ctrl->flags |= SNOR_FLG_STRIPE;
        } else {
            _snor_reset_chip(chip);
        }
    }
}

/**
 *  @brief             Reset a flash chip.
 *  @param chip        Pointer to chip structure
 *
 *  @return            None
 */
static void _snor_reset_chip(snor_chip_t *chip)
{
    snor_ctrl_t * const ctrl = chip->ctrl;

    chip->cfg.bus_proto = SNOR_BUSPROTO_1_1_1;
    if (ctrl->funcs.cfg_bus != NULL) {
        ctrl->funcs.cfg_bus(ctrl, &chip->cfg);
    }
    // need to re-configure the chip
    // TODO, since reset is called after error, maybe we should down grade the bus?
    if (chip->flags & SNOR_CFLG_PRESENT) {
        chip->post_ident(chip);
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
