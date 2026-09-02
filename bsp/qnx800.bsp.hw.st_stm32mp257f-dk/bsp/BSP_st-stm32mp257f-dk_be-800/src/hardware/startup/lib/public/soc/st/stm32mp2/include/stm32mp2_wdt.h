/*
 * $QNXLicenseC:
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
 * $
 */

#ifndef STM32_WDT_H_
#define STM32_WDT_H_

#include <stdint.h>

/**
 * @file stm32mp2_wdt.h
 * @brief STM32MP2 watchdog driver interface doing SMC calls to TF-A for watchdog
 *  maintanance.
 */

int stm32mp2_wdt_reset();
int stm32mp2_wdt_stop();

#endif /* STM32_WDT_H_ */
