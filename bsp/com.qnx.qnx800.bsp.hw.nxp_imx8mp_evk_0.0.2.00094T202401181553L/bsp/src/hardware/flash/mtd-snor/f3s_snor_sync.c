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
 *  @brief             Sync callout for SPI serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Sync flags.
 *  @param offset      dwiSync Lock offset.
 *
 *  @return            EOK --erase complete
 *                     EAGAIN --erase inprogress
 *                     Otherwise --failure
 */
/*
 * This is the sync callout for SPI serial NOR flash.
 */
int32_t f3s_snor_sync(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    uint8_t     sr[SNOR_MAX_CS];
    int         nsr, i;

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    ctrl = chip->ctrl;

    nsr = (ctrl->flags & SNOR_FLG_STRIPE) ? chip->cfg.ncs : 1;
    // has flag status register?
    if (chip->dcaps & SNOR_DCAPS_FSTATUS) {
        if (snor_read_fsr(chip, sr) != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: snor_read_fsr failed", __func__);
            return (EIO);
        }

        if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
            ctrl->funcs.dstripe(ctrl, sr, nsr, 0);
        }
        for (i = 0; i < nsr; i++) {
            if (sr[i] & (SNOR_FLGSTS_EFAIL | SNOR_FLGSTS_PFAIL | SNOR_FLGSTS_EPROCT)) return (EIO);
            if (!(sr[i] & SNOR_FLGSTS_RDY)) return (EAGAIN);
        }

        return (EOK);
    }

    // use status register
    if (snor_read_register(chip, SNOR_CMD_RDSR, 0, 0, sr, nsr) != EOK) return (EIO);
    if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
        ctrl->funcs.dstripe(ctrl, sr, nsr, 0);
    }
    for (i = 0; i < nsr; i++) {
        if (sr[i] & SNOR_STS_WIP) return (EAGAIN);
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
