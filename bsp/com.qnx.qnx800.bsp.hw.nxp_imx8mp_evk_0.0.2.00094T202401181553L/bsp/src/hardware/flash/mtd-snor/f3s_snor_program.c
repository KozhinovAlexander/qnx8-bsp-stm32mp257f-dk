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
 *  @return            Written byte count --success otherwise negative value.
 */
int32_t f3s_snor_program(f3s_dbase_t *dbase,
                         f3s_access_t *access,
                         uint32_t flags,
                         uint32_t offset,
                         int32_t size,
                         uint8_t *buffer)
{
    snor_ctrl_t* const ctrl = (snor_ctrl_t *)access->socket.memory;
    snor_chip_t *chip;
    snor_cmd_t  cmd;
    int32_t     pagesz;
    uint8_t     *tmpbuf = NULL;
    uint8_t     *wrbuf;
    int32_t     wrlen;
    int32_t     rdlen;
    uint32_t    tmo;
    int32_t     mask;
    int32_t     pads, tail = 0;
    int         err = EOK;

    if (ctrl->funcs.write == NULL) {
        errno = ENOTSUP;
        return (-1);
    }

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, &size);
    if (chip == NULL) {
        errno = ERANGE;
        return (-1);
    }

    if (chip->pagesz < chip->align) {
        errno = ENOTSUP;
        return (-1);
    }
    pagesz = (int32_t)chip->pagesz;

    // hardware can automatically issue WREN command?
    if (!(ctrl->ccaps & SNOR_CCAPS_PPAWREN)) {
        if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) {
            errno = EIO;
            return (-1);
        }
    }

    size = min(size, pagesz - (int32_t)(offset & ((uint32_t)pagesz - 1)));    // maximum length within page

    // aligment can only be power of two and <= write buffer
    mask = chip->align - 1;
    pads = (int32_t)(offset & (uint32_t)mask);
    if ((pads != 0) || (size & mask)) {
        tmpbuf = malloc((size_t)pagesz);
        if (tmpbuf == NULL) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: malloc failed", __func__);
            return (-1);
        }
        memset(tmpbuf, 0xff, (size_t)pagesz);
        memcpy(&tmpbuf[pads], buffer, (size_t)size);
        wrbuf = tmpbuf;
        wrlen = size + pads;
        // now address is aligned, check length alignment
        const int32_t mwl = wrlen & mask;
        if (mwl) {
            tail   = (int32_t)chip->align - mwl;
            wrlen += tail;
        }
        // We need to call page function again to get offset after the padding,
        access->service->page(&access->socket, 0, offset - (uint32_t)pads, &wrlen);
    } else {
        wrbuf = buffer;
        wrlen = size;
    }

    SNOR_SET_CMD(cmd, &chip->op_wr, &chip->wrcfg, chip->offset);
    size = ctrl->funcs.write(ctrl, &cmd, wrbuf, wrlen);
    if (size <= 0) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: write failed", __func__);
        if (tmpbuf != NULL) {
            free(tmpbuf);
        }
        return (size);
    }

    // hardware can automatically poll status register?
    if (!(ctrl->ccaps & SNOR_CCAPS_PPASP)) {
        // no page program should exceed 100ms
        tmo = (chip->pptt && chip->ppmt) ? ((uint32_t)chip->pptt * (uint32_t)chip->ppmt) : 100000;
        tmo *= 10;
        while (--tmo) {
            err = chip->flash->v2sync(dbase, access, flags, offset);
            if (err != EAGAIN) break;
            nanospin_ns(100);
        }

        if (err != EOK) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: poll failed[%d]", __func__, err);
            errno = (err == EAGAIN) ? EIO : err;
            if (tmpbuf != NULL) {
                free(tmpbuf);
            }
            return (-1);
        }
    }

    if (!(flags & F3S_VERIFY_WRITE)) {
        size -= (pads + ((size < wrlen) ? 0 : tail));   // FIXME! can't simply set tail as "0"?
        if (tmpbuf != NULL) {
            free(tmpbuf);
        }
        return (size);
    }

    if (tmpbuf == NULL) {
        tmpbuf = malloc((size_t)pagesz);
        if (tmpbuf == NULL) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: malloc failed", __func__);
            return (-1);
        }
    }

    // We should be able to read back as many data as we are able to write
    rdlen = chip->flash->v2read(dbase, access, flags, offset, wrlen, tmpbuf);
    if (rdlen != wrlen) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: flash read failed", __func__);
        if (tmpbuf != NULL) {
            free(tmpbuf);
        }
        return (-1);
    }

    size -= (pads + ((size < wrlen) ? 0 : tail));   // FIXME! can't simply set tail as "0"?

    if (memcmp(tmpbuf, buffer, (size_t)size)) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
                    "(devf  t%d::%s:%d) program verify error "
                    "between offset 0x%x and 0x%x, size = %d",
                    pthread_self(), __func__, __LINE__,
                    offset, offset + (uint32_t)size, size);
        errno = EIO;
        size  = -1;
    }

    if (tmpbuf != NULL) {
        free(tmpbuf);
    }

    return (size);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
