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
 *  @brief             Write data to register.
 *  @param chip        Flash chip handle.
 *  @param opcode      OP code.
 *  @param addr        Register address.
 *  @param adrlen      Register address length.
 *  @param buf         Data buffer.
 *  @param len         Data length
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_write_register(snor_chip_t* const chip, const uint8_t opcode,
                const uint32_t addr, const uint8_t adrlen, uint8_t* const buf, const int len)
{
    snor_ctrl_t* const ctrl = chip->ctrl;
    snor_cmd_t  cmd;
    snor_op_t   wrop = { .opcode = opcode, .dcycle = 0, .adrlen = adrlen };

    SNOR_SET_CMD(cmd, &wrop, &chip->cfg, addr);

    if (ctrl->funcs.write_reg != NULL) {
        return ctrl->funcs.write_reg(ctrl, &cmd, buf, len);
    }

    if (ctrl->funcs.write != NULL) {
        if (ctrl->funcs.write(ctrl, &cmd, buf, len) == len) return (EOK);
        return (EIO);
    }

    return (ENOTSUP);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
