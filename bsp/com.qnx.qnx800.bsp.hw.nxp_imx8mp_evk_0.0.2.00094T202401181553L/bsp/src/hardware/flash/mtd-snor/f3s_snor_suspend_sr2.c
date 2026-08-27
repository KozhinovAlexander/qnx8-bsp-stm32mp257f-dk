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
 *  @brief             Erase suspend callout for SPI serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Erase suspend flags.
 *  @param offset      Erase suspend offset.
 *
 *  @return            EOK --suspended successfully
 *                     ECANCELED --Erase complete, suspended unnecessary
 *                     Otherwise --failure
 */
int32_t f3s_snor_suspend_sr2(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    uint8_t     sr[SNOR_MAX_CS];
    int         loop, nsr, rdy, sus, i;

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    if (snor_write_cmd(chip, chip->op_es) != EOK) return (EIO);

    ctrl = chip->ctrl;

    nsr = (ctrl->flags & SNOR_FLG_STRIPE) ? chip->cfg.ncs : 1;

    // wait for all chips to be ready
    // TODO, maximum 1ms, should we use the erase to suspend latency set by chip?
    for (loop = 1000; loop > 0; loop--) {
        if (snor_read_register(chip, SNOR_CMD_RDSR, 0, 0, sr, nsr) != EOK) return (EIO);

        if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
            ctrl->funcs.dstripe(ctrl, sr, nsr, 0);
        }
        rdy = 0;
        for (i = 0; i < nsr; i++) {
            if (!(sr[i] & SNOR_STS_WIP)) {
                rdy++;
            }
        }
        // All chips ready?
        if (rdy == nsr) {
            if (snor_read_register(chip, chip->asr2, 0, 0, sr, nsr) != EOK) return (EIO);
            if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
                ctrl->funcs.dstripe(ctrl, sr, nsr, 0);
            }
            sus = 0;
            for (i = 0; i < nsr; i++) {
                if (sr[i] & (1 << chip->sr2_esbit)) {
                    sus++;
                }
            }

            snor_slogf(_SLOG_INFO, ctrl->verbosity, 3, "(devf  t%d::%s:%d) erase suspend %s",
                    pthread_self(), __func__, __LINE__, (sus != 0) ? "OK" : "canceled");
            return (sus != 0) ? EOK : ECANCELED;
        }

        nanospin_ns(1000);
    }

    snor_slogf(_SLOG_ERROR, 0, 0, "(devf  t%d::%s:%d) erase suspend failed", pthread_self(), __func__, __LINE__);

    return (EIO);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
