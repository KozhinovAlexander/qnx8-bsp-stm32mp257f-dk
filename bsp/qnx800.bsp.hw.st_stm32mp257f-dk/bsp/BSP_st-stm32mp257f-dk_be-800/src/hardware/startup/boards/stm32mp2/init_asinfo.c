/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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
 * STM32MP257F-DK board physical address space information
 */

#include <startup.h>

/**
 * STM32MP257F-DK startup source file.
 *
 * @file       init_asinfo.c
 * @addtogroup startup
 * @{
 */

/**
 * Initialize the asinfo section of the system page.
 *
 * @param mem Offset of the memory entry in the section.
 */
void init_asinfo(const unsigned mem)
{
    /*
     * Add the GPU controller registers
     */
    // as_add(0x38000000, 0x38007FFF, AS_ATTR_DEV+AS_OVERLAY_IO, "gpu-reg-base-3D", mem);
    // as_add(0x38008000, 0x3800FFFF, AS_ATTR_DEV+AS_OVERLAY_IO, "gpu-reg-base-2D", mem);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/init_asinfo.c $ $Rev: 984580 $")
#endif
