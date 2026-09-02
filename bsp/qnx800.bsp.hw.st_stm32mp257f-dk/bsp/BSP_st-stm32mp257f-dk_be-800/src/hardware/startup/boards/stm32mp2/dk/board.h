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
#define STM32MP2_RCC_BASE_ADDR    (0x44200000)

/** Core counter input clock (in MHz) */
#define STM32MP2_HSE_CLOCK_FREQ   40'000'000

/** STM32MP2 USART2 base address and size */
#define STM32_USART2_BASE_ADDR  (0x400e0000)  /* RM0457 Rev 5, page 248/5881 */
#define STM32_UART_SIZE         (0x400)  /* RM0457 Rev 5, page 248/5881 */

/*!
 * @name QNX SDRAM memory configuration
 */
/*@{*/
/** DRAM0 base address */
#define STM32MP2_DRAM0_BASE       (0x80000000)
/** DRAM0 size in bytes */
#define STM32MP2_DRAM0_SIZE       GIG(4UL)
/** DRAM0 TF-A stage size in bytes (BL2/BL31 etc.) */
#define STM32MP2_DRAM0_TFA_SIZE   MEG(256UL)
/** DRAM0 base address available for OS */
#define STM32MP2_DRAM0_OS_BASE    (STM32MP2_DRAM0_BASE + STM32MP2_DRAM0_TFA_SIZE)
/** DRAM0 base size available for OS */
#define STM32MP2_DRAM0_OS_SIZE    (STM32MP2_DRAM0_SIZE - STM32MP2_DRAM0_TFA_SIZE)
/*@}*/

/** Device electronic signature registers */
#define BSEC_BASE                 (0x44000000)
#define BSEC_FVR5                 (BSEC_BASE + 0x014)
#define BSEC_FVR6                 (BSEC_BASE + 0x018)
#define BSEC_FVR7                 (BSEC_BASE + 0x01C)
#define BSEC_FVR9                 (BSEC_BASE + 0x024)
#define BSEC_FVR102               (BSEC_BASE + 0x198)
#define BSEC_FVR122               (BSEC_BASE + 0x1E8)

/** STM32MP2 GIC addresses */
#define GICD_PADDR            (0x4AC10000)
#define GICC_PADDR            (0x4AC20000)
#define GICH_PADDR            (0x4AC40000)
#define GICV_PADDR            (0x4AC60000)

/** STM32MP2 independent-watchdog addresses */
#define STM32MP2_IWDG1_BASE   (0x44010000)
#define STM32MP2_IWDG2_BASE   (0x44020000)
#define STM32MP2_IWDG3_BASE   (0x44030000)
#define STM32MP2_IWDG4_BASE   (0x44040000)

/* Use iwdg1 watchdog */
#define STM32MP2_IWDG_BASE   STM32MP2_IWDG1_BASE

#endif  /* BOARD_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/board.h $ $Rev: 985114 $")
#endif
