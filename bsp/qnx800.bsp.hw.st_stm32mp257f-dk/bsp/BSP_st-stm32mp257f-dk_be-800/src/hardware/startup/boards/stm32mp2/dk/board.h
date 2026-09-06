/*
 * Copyright (c) 2016, 2022-2023, BlackBerry Limited.
 * Copyright 2022-2023 NXP
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


#ifndef BOARD_H_
#define BOARD_H_

#include <soc/st/stm32mp2/include/stm32mp257.h>

#ifndef __ASM__
#include "stm32mp2_startup.h"
#endif

/** Core counter input clock (in MHz) */
#define STM32MP2_HSE_CLOCK_FREQ   40'000'000

/*!
 * @name QNX SDRAM memory configuration
 */
/*@{*/
/** DRAM0 base address */
#define STM32MP2_DRAM0_BASE       (0x80000000UL)
/** DRAM0 size in bytes */
#define STM32MP2_DRAM0_SIZE       GIG(4UL)
/** DRAM0 TF-A stage size in bytes (BL2/BL31 etc.) */
#define STM32MP2_DRAM0_TFA_SIZE   MEG(256UL)
/** DRAM0 base address available for OS */
#define STM32MP2_DRAM0_OS_BASE    (STM32MP2_DRAM0_BASE + STM32MP2_DRAM0_TFA_SIZE)
/** DRAM0 base size available for OS */
#define STM32MP2_DRAM0_OS_SIZE    (STM32MP2_DRAM0_SIZE - STM32MP2_DRAM0_TFA_SIZE)
/*@}*/

/* Use iwdg1 watchdog */
#define STM32MP2_IWDG_BASE   STM32MP2_IWDG1_BASE


#define STM32MP2_ETH1_MAC_NAME     "dwmac"

#endif  /* BOARD_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/board.h $ $Rev: 985114 $")
#endif
