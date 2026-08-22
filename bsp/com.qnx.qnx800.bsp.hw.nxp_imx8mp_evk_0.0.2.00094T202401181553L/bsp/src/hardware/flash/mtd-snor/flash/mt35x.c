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

#define MT35X_ID_LEN        4

#define MT35x_MANID         0x2C    // JEDEC ID, Micron
#define MT35x_TYPE_33v      0x5A    // 3.3v type
#define MT35x_TYPE_18v      0x5B    // 1.8v type

#define MT35X_OP_WVCR       0x81    // write volatile configuration register

static int _mt35x_set_protocol(struct _snor_chip_t *chip, uint32_t proto);

/**
 *  @brief             Ident callout for Micron MT35X serial NOR flash.
 *  @param dbase       F3S data base handle.
 *  @param access      F3S access handle.
 *  @param flags       Ident flags.
 *  @param cs          Chip select
 *
 *  @return            EOK --success otherwise fail.
 */
int32_t f3s_mt35x_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs)
{
    snor_ctrl_t *ctrl;
    snor_chip_t *chip;
    uint8_t     ids[MT35X_ID_LEN];

    if (access == NULL) return (ENODEV);

    ctrl = (snor_ctrl_t *)access->socket.memory;

    chip = &ctrl->chip[cs];

    if ((chip->cfg.bus_proto != SNOR_BUSPROTO_1_1_1) &&
        (chip->cfg.bus_proto != SNOR_BUSPROTO_8_8_8_DTR)) return (ENOTSUP);

    if ((chip->vid != 0) && (chip->did != 0)) {
        ids[0] = chip->vid;
        ids[1] = chip->did;
    } else {
        if (snor_read_id(chip, ids, MT35X_ID_LEN) != EOK) return (EIO);
    }

    if ((ids[0] == MT35x_MANID) && ((ids[1] == MT35x_TYPE_33v) || (ids[1] == MT35x_TYPE_18v))) {
        chip->hcaps = SNOR_HCAPS_RD_1_1_1 | SNOR_HCAPS_RD_1_1_1_FAST | SNOR_HCAPS_RD_8_8_8 | SNOR_HCAPS_RD_OCTAL |
                      SNOR_HCAPS_PP_1_1_1 | SNOR_HCAPS_PP_8_8_8 | SNOR_HCAPS_PP_1_8_8 |
                      SNOR_HCAPS_DTR | SNOR_HCAPS_DQS;

        if (f3s_sfdp_ident(dbase, access, flags, cs) != EOK) return (ENOTSUP);

        chip->flags |= SNOR_CFLG_DLTBS | SNOR_CFLG_DLBBS | SNOR_CFLG_PSLOCK | SNOR_CFLG_DNLOCK;
    } else {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "%s: unsupported ID[%02x:%02x]", __func__, ids[0], ids[1]);
        return (ENOTSUP);
    }

    if (dbase != NULL) {
        dbase->jedec_hi    = ids[0];
        dbase->jedec_lo    = (uint16_t)(ids[1] << 8);
        dbase->jedec_lo    = (uint16_t)ids[2];
        dbase->name        = "MT35X";
    }

    chip->set_protocol = _mt35x_set_protocol;

    return (EOK);
}

/**
 *  @brief             Set protocol callout for Micron MT35X serial NOR flash.
 *                     Use Volatile Configuration Register to configure bus
 *                     Octal, DTR, DQS as well as driver strength.
 *  @param chip        Chip handle.
 *  @param proto       Protocol.
 *
 *  @return            EOK --success otherwise fail.
 */
static int _mt35x_set_protocol(struct _snor_chip_t* const chip, const uint32_t proto)
{
    const snor_ctrl_t* const ctrl = chip->ctrl;
    uint8_t     vcr = 0;
    uint32_t    flg_4ba = 0;
    uint8_t     alen = 3;

    if (ctrl->verbosity > 3) {
        snor_slogf(_SLOG_INFO, ctrl->verbosity, 1, "%s: set protocol %x%s%s", __func__,
            proto & SNOR_BUSPROTO_BUS_MASK,
            (proto & SNOR_BUSPROTO_DTR_MODE) ? "-DTR" : "",
            (proto & SNOR_BUSPROTO_DQS) ? "-DQS" : "");
    }

    if (chip->flags & SNOR_CFLG_4B_ADDR) {
        alen = 4;
    }

    /* driver strength @ byte 3 */
    if (chip->drv_type != 0) {
        if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
        if (snor_write_register(chip, MT35X_OP_WVCR, 3, alen, &chip->drv_type, 1) != EOK) return (EIO);
    }

    if (ctrl->hcaps & SNOR_HCAPS_DQS) {
        vcr = 0x20;
    }

    /* bus mode */
    if ((proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_8_8_8) {
        if (proto & SNOR_BUSPROTO_DTR_MODE) {
            vcr |= 0xC7;
            flg_4ba = SNOR_CFLG_4B_ADDR;
        } else {
            return (ENOTSUP);
        }
    } else {
        vcr |= 0xDF;
    }

    if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
    if (snor_write_register(chip, MT35X_OP_WVCR, 0, alen, &vcr, 1) != EOK) return (EIO);

    chip->flags |= flg_4ba;

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
