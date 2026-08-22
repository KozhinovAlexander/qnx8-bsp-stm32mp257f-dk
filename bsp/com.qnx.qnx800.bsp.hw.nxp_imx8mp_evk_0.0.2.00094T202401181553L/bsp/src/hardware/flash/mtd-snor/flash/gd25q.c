/*
 * Copyright (c) 2023, BlackBerry Limited.
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

#define GD25Q_ID_LEN        4
#define GD25Q_PAGE_SIZE     256

#define GD25Q_MANID         0xC8    /* JEDEC ID */
#define GD25Q_DID0          0x40
#define GD25Q_DID1          0x18


/**
 *  @brief             Ident callout for GigaDevice GD25Q serial NOR flash.
 *  @param dbase       F3S data base handle.
 *  @param access      F3S access handle.
 *  @param flags       Ident flags.
 *  @param cs          Chip select
 *
 *  @return            EOK --success otherwise fail.
 */
int32_t f3s_gd25q_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs)
{
    snor_ctrl_t *ctrl;
    snor_chip_t *chip;
    uint8_t     ids[GD25Q_ID_LEN];

    if (access == NULL) return (ENODEV);

    ctrl = (snor_ctrl_t *)access->socket.memory;

    chip = &ctrl->chip[cs];

    if (chip->cfg.bus_proto != SNOR_BUSPROTO_1_1_1) return (ENOTSUP);

    if (snor_read_id(chip, ids, GD25Q_ID_LEN) != EOK) return (EIO);

    if ((ids[0] == GD25Q_MANID) && (ids[1] == GD25Q_DID0) && (ids[2] == GD25Q_DID1)) {
        chip->hcaps = SNOR_HCAPS_RD_1_1_1 | SNOR_HCAPS_RD_1_1_1_FAST | SNOR_HCAPS_PP_1_1_1;

        if (f3s_sfdp_ident(dbase, access, flags, cs) != EOK) return (ENOTSUP);

        chip->pagesz = GD25Q_PAGE_SIZE;

        chip->dcaps |= SNOR_DCAPS_PSR | SNOR_DCAPS_ESR;
        chip->op_pr = 0x7A;
        chip->op_ps = 0x75;
        chip->op_er = 0x7A;
        chip->op_es = 0x75;

        // Add chip specific erase suspend calllout, generic resume callout should work.
        chip->asr2  = 0x35;
        chip->sr2_esbit = 7;
    } else {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "%s: unsupported ID[%02x:%02x]", __func__, ids[0], ids[1]);
        return (ENOTSUP);
    }

    if (dbase != NULL) {
        dbase->jedec_hi    = ids[0];
        dbase->jedec_lo    = (uint16_t)(ids[1] << 8);
        dbase->jedec_lo   |= (uint16_t)ids[2];
        dbase->name        = "GD25Q";
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
