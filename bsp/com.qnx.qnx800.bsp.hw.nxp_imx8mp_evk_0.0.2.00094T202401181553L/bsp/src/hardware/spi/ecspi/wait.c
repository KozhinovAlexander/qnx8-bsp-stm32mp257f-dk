/*
 * Copyright (c) 2010,2023, BlackBerry Limited.
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
 */

#include "ecspi.h"

/**
 *  @brief             Wait for SPI transfer complete.
 *  @param  spi        SPI driver handler.
 *
 *  @return            EOK --success otherwise fail.
 */
int ecspi_wait(ecspi_t *const spi, const uint32_t len)
{
    struct timespec timeout;
    const uint64_t  ns_timeout = spi->dtime * len * 1000 * 50ull;   /* timeout in ns. 50 times for time out */
    const spi_bus_t *const bus = spi->bus_node;
    int status = 0;

    clock_gettime(CLOCK_MONOTONIC, &timeout);
    nsec2timespec(&timeout, timespec2nsec(&timeout) + ns_timeout);

    while (1) {
        status = sem_timedwait_monotonic(bus->sem, &timeout);
        if (status == -1) {
            spi_slogf(_SLOG_ERROR, "%s: sem_timedwait_monotonic failed: %s",
                                    __func__, strerror(errno));
            status = errno;
            break;
        }

        /* Process SPI interrrupts */
        status = process_interrupts(spi);
        InterruptUnmask(spi->irq, spi->iid);
        if (status != 0) {
            spi_slogf(_SLOG_DEBUG2, "%s: Transfer is not completed yet", __func__);
        } else {
            spi_slogf(_SLOG_DEBUG2, "%s: Transfer is completed.", __func__);
            status = EOK;
            break;
        }
    }

    return status;
}

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/spi/ecspi/wait.c $ $Rev: 980075 $")
#endif
