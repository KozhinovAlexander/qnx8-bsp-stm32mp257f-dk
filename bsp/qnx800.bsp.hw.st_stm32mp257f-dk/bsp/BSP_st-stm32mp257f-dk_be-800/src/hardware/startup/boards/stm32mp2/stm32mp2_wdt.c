/*
 * Copyright 2026 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
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

/*
 * Independent watchdog operations for STM32MP2
 */

#include <startup.h>
#include <stdint.h>

#if defined(__aarch64__)
#include <aarch64/psci.h>
#else
#include <arm/psci.h>
#endif

#include "board.h"
#include <soc/st/stm32mp2/include/stm32mp2_wdt.h>

/**
 * STM32MP257F-DK startup source file.
 *
 * @file       stm32mp2_wdt.c
 * @addtogroup startup
 * @{
 */

/**
* arm_wdt: watchdog {
*   compatible = "arm,smc-wdt";
*   arm,smc-id = <0xbc000000>;
*   status = "disabled";
* };
*/
#define PSCI_ARM_WDT_SMC_ID   0xbc000000

enum smcwd_call {
  SMCWD_INIT          = 0,
  SMCWD_SET_TIMEOUT   = 1,
  SMCWD_ENABLE        = 2,
  SMCWD_PET           = 3,
  SMCWD_GET_TIMELEFT  = 4,
};

int stm32mp2_wdt_reset()
{
  return (int)psci_call(PSCI_ARM_WDT_SMC_ID, SMCWD_PET, 0, 0);
}

int stm32mp2_wdt_stop()
{
  return (int)psci_call(PSCI_ARM_WDT_SMC_ID, SMCWD_ENABLE, 0, 0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/stm32mp2_wdt.c $ $Rev: 984580 $")
#endif
