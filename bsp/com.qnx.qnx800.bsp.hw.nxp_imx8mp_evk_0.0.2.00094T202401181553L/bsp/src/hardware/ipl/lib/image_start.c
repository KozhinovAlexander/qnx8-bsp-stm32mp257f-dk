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
 * image_start: Hand execution over to the address of the image
*/

#include "ipl.h"

//
// Start the image which has been loaded into memory
//

int image_start(const paddr_t addr)
{

    copy((paddr_t)(&startup_hdr), addr, sizeof(startup_hdr));

    //
    //  Options here include custom jump functions,
    //  cast as a function call? use the longjmp call
    //

    jump(startup_hdr.startup_vaddr);

    return (-1);
}

static void load_addr (const unsigned long fdt_addr)
{
    __asm__ __volatile__ ("ldr x3, %0" : : "m"(fdt_addr) : "x3");
}

int image_start_with_fdt (const unsigned long addr, const unsigned long fdt_addr) {

    copy ((unsigned long)(&startup_hdr), addr, sizeof(startup_hdr));

    load_addr (fdt_addr);
    //
    //  Options here include custom jump functions,
    //  cast as a function call? use the longjmp call
    //
    jump (startup_hdr.startup_vaddr);
    return (-1);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/image_start.c $ $Rev: 975396 $")
#endif
