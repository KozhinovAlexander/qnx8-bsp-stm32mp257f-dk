/*
 * Copyright (c) 2007, 2008, 2022-2023, BlackBerry Limited.
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


/*
 *  image_scan: Scan through memory looking for an image
 */

#include "ipl.h"

char                    scratch [512];
struct startup_header   startup_hdr;

//
//     Scan 1k boundaries for the image identifier byte and
//     then does a checksum on the image.
//

paddr_t image_scan(paddr_t start, const paddr_t end, const int docksum)
{
    paddr_t     lastaddr = (paddr_t)-1;
    uint16_t    lastver = 0;

    //
    // We assume that the images will all start on a 4 byte boundary
    //

    while (start < end) {

        copy((paddr_t)(&startup_hdr), start, sizeof(startup_hdr));

        //
        //  No endian issues here since stored "naturally"
        //

        if (startup_hdr.signature != STARTUP_HDR_SIGNATURE) {
            start += sizeof(uint32_t);
            continue;
        }

        if (docksum) {
            //
            // There are two checksums, one for the startup
            // and one for the image.  Check both of them.
            //

            if (checksum(start, startup_hdr.startup_size) != 0) {
                start += sizeof(uint32_t);
                continue;
            }

            if (checksum(start + startup_hdr.startup_size,
                      startup_hdr.stored_size - startup_hdr.startup_size) != 0) {
                start += sizeof(uint32_t);
                continue;
            }
        }

        //
        // Stash the version and address and continue looking
        // for something newer than we are (jump ahead by
        // startup_hdr.stored_size)
        //

        if (startup_hdr.version > lastver) {
            lastver = startup_hdr.version;
            lastaddr = start;
        }

        start += startup_hdr.stored_size;
    }

    return (lastaddr);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/image_scan.c $ $Rev: 975396 $")
#endif
