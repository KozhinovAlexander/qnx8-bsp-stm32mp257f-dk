/*
 * Copyright (c) 2014, 2022-2023, BlackBerry Limited.
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

#include <ipl.h>

/*
 * Appends a startup_info_hdr to the end of the startup header of the image at
 * imageaddr.
 *
 * Returns: true (!0) or false (0) on success or failure respectively
 */
int image_add_info(void *const imaddr, const struct startup_info_hdr *const info)
{
    struct startup_header   *sh      = imaddr;
    const paddr_t           ram_addr = sh->ram_paddr + sh->paddr_bias;
    struct startup_info_hdr *sih;

    sih = (struct startup_info_hdr *)((char *)ram_addr + ((char *)&sh->info[0] - (char *)sh));

    /* advance to the next free 'info' slot */
    while (sih->size) {
        sih = (struct startup_info_hdr *)((char *)sih + sih->size);
    }

    if ((((paddr_t)sih - ram_addr) + info->size) > sizeof(struct startup_header)) {
        return (0);     /* no room to add another info structure */
    }
    copy((paddr_t)sih, (paddr_t)info, (size_t)info->size);

    return (1);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/image_add_info.c $ $Rev: 975396 $")
#endif
