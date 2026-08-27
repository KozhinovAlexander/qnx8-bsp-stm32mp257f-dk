/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

#ifndef STM32_WDOG_H_
#define STM32_WDOG_H_

#define STM32MP2_IWDG_BASE	0x44010000

/* IWDG registers offsets */
#define IWDG_KR_OFFSET		0x00U
#define IWDG_PR_OFFSET		0x04U
#define IWDG_RLR_OFFSET		0x08U

/* Registers values */
#define IWDG_KR_EN_WRITE_KEY	0x5555U
#define IWDG_KR_RELOAD_KEY	0xAAAAU
#define IWDG_KR_ENABLE_KEY	0xCCCCU

#define IWDG_PRESCALER_MIN	2U
#define IWDG_PRESCALER_MAX	10U
#define IWDG_RLR_BITS		12U

#endif /* STM32_WDOG_H_ */
