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

#ifndef __ASM__
#include "stm32mp2_startup.h"
#endif


/** STM32MP2 SoC cores number */
#define STM32MP2_CPU_CORES_NUMBER	2

/* STM32MP25 RCC Register base address RM0457 Rev 5 243/5881 */
#define STM32MP2_RCC_BASE_ADDR		0x44200000

/** Core counter input clock (in MHz) */
#define STM32MP2_HSE_CLOCK_FREQ		40'000'000

/*!
 * @name QNX SDRAM memory configuration
 */
/*@{*/
/** SDRAM0 base address */
#define STM32MP2_SDRAM0_BASE		0x80000000
/** SDRAM0 size in bytes */
#define STM32MP2_SDRAM0_SIZE		GIG(4UL)
/*@}*/



#endif  /* BOARD_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/board.h $ $Rev: 985114 $")
#endif
