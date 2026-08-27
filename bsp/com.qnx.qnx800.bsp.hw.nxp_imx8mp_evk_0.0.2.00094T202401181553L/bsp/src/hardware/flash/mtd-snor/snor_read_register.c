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
 *  @brief             Read SPI serial NOR flash register.
 *  @param chip        Flash chip handle.
 *  @param opcode      Read register OP code
 *  @param addr        Register address
 *  @param adrlen      Register address length
 *  @param buf         Register buffer
 *  @param len         Register length
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_read_register(snor_chip_t* const chip, const uint8_t opcode,
                const uint32_t addr, const uint8_t adrlen, uint8_t* const buf, const int len)
{
    snor_ctrl_t* const ctrl = chip->ctrl;
    snor_op_t   rdr = { .opcode = opcode, .dcycle = 0, .adrlen = adrlen };
    snor_cmd_t  cmd;
    int         err = ENOTSUP;
    uint8_t     *pb = buf;
    int         rlen = len;

    if (len <= 0) return (EINVAL);

    // 8 dummy cycles for octal mode
    if ((chip->cfg.bus_proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_8_8_8) {
        rdr.dcycle = 8;
        if (len & 1) {
            pb = malloc((size_t)(len + 1));
            if (pb == NULL) return (ENOMEM);
            rlen = len + 1;
        }
    }

    SNOR_SET_CMD(cmd, &rdr, &chip->cfg, addr);

    if (ctrl->funcs.read_reg != NULL) {
        err = ctrl->funcs.read_reg(ctrl, &cmd, pb, rlen);
    } else if (ctrl->funcs.read != NULL) {
        if (ctrl->funcs.read(ctrl, &cmd, pb, rlen) != rlen) {
            err = EIO;
        } else {
            err = EOK;
        }
    }

    if (pb != buf) {
        if (err == EOK) {
            memcpy(buf, pb, (size_t)len);
        }
        free(pb);
    }

    return (err);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
