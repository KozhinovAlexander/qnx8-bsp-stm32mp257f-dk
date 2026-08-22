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
 *  @brief             Read status register.
 *  @param chip        Flash chip handle.
 *  @param sr          Pointer to chip status.
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_read_sr(snor_chip_t* const chip, uint8_t *sr)
{
    uint8_t buf[2];     /* read two bytes, use the second byte */
    int     err;

    err = snor_read_register(chip, SNOR_CMD_RDSR, 0, 0, buf, sizeof(buf));
    if (err == EOK) {
        *sr = buf[1];
    }

    return (err);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
