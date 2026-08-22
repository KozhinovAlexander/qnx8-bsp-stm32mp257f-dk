/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2022 NXP
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
 * init_raminfo.c
 * Tell syspage about our RAM configuration
 */
#include <startup.h>
#include "board.h"
#include "stm32mp2_startup.h"

/**
 * STM32MP2 startup source file.
 *
 * @file       stm32mp2_init_raminfo.c
 * @addtogroup startup
 * @{
 */

/**
 * Add RAM area information to the system page.
 */
void stm32mp2_init_raminfo(void)
{
    /*
     * 4GB RAM initialization:
     * | ----------------------- 4 GB ----------------------- |
     * | SDMA (4K) | RPMsg-lite (512K) | -- SYSRAM ---------- |
     */
    add_ram(STM32MP2_SDRAM0_BASE, STM32MP2_SDRAM0_SIZE);
    /* Add 4 KB /memory/dma region. This region is not used by QNX and is dedicated to DMA. */
    // as_add(IMX_SDRAM0_BASE, IMX_SDRAM0_BASE + KILO(4) - 1, AS_ATTR_NONE, "dma", as_default());
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/stm32mp2_init_raminfo.c $ $Rev: 984580 $")
#endif
