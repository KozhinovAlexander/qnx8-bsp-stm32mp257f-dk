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
 *  @brief             Flash device software reset.
 *  @param chip        Flash chip handle.
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_reset(snor_chip_t* const chip)
{
    int32_t sts = EOK;

    // TODO, handle other reset sequence
    if ((chip->ssrs == 0) || (chip->ssrs & (1 << 4))) {
        if (snor_write_cmd(chip, SNOR_CMD_SRSTEN) != EOK) return (EIO);

        sts = snor_write_cmd(chip, SNOR_CMD_SRST);

        delay(1);
    }

    return (sts);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
