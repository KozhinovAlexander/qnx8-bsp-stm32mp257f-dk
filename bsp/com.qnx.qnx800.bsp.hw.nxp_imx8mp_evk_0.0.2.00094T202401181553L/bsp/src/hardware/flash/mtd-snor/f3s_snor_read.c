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
 *  @brief             Page program callout for SPI serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Lock flags.
 *  @param offset      Lock offset.
 *  @param size        Buffer size.
 *  @param buffer      Buffer pointer.
 *
 *  @return            Read byte count --success otherwise negative value.
 */
/*
 * This is the read callout for SPI serial NOR flash.
 */
int32_t f3s_snor_read(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset, int32_t size, uint8_t *buffer)

{
    snor_ctrl_t* const ctrl = (snor_ctrl_t *)access->socket.memory;
    snor_chip_t* chip;
    snor_cmd_t  cmd;
    int32_t     len;
    uint32_t    mask;
    int32_t     pads, tail = 0;
    uint8_t     *tbuf;

    if (ctrl->funcs.read == NULL) {
        errno = ENOTSUP;
        return (-1);
    }

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, &size);
    if (chip == NULL) {
        errno = ERANGE;
        return (-1);
    }

    mask = (uint32_t)(chip->align - 1);
    pads = (int32_t)(offset & mask);
    // miss aligned address or length?
    if ((pads != 0) || (size & (int32_t)mask)) {
        len = size + pads;
        const int32_t head = len & (int32_t)mask;
        if (head) {
            tail = (int32_t)chip->align - head;
            len += tail;
        }

        // We need to call page function again to get offset after the padding,
        access->service->page(&access->socket, 0, offset - (uint32_t)pads, &len);
        tbuf = malloc((size_t)len);
        if (tbuf == NULL) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: malloc buffer failed", __func__);
            errno = ENOMEM;
            return (-1);
        }

        SNOR_SET_CMD(cmd, &chip->op_rd, &chip->rdcfg, chip->offset);
        if (ctrl->funcs.read(ctrl, &cmd, tbuf, len) != len) {
            len = -1;
            errno = EIO;
        } else {
            len -= pads + tail;
            memcpy(buffer, &tbuf[pads], (size_t)len);
        }
        free(tbuf);

        return (len);
    }

    SNOR_SET_CMD(cmd, &chip->op_rd, &chip->rdcfg, chip->offset);

    return ctrl->funcs.read(ctrl, &cmd, buffer, size);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
