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
 */


#ifndef STM32MP2_STARTUP_H_
#define STM32MP2_STARTUP_H_

// #include <soc/nxp/imx8/mp/mx8mp.h>

/**
 * STM32MP257F-DK startup source file.
 *
 * @file       stm32mp2_startup.h
 * @addtogroup startup
 * @{
 */

extern void stm32mp2_init_uart(unsigned channel, const char *init, const char *defaults);
extern void stm32mp2_uart_put_char(int);

extern struct callout_rtn stm32mp2_uart_display_char;
extern struct callout_rtn stm32mp2_uart_poll_key;
extern struct callout_rtn stm32mp2_uart_break_detect;

void stm32mp2_init_raminfo(void);

extern void init_pcie_ext_msi_controller(void);

#endif /* STM32MP2_STARTUP_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/stm32mp2_startup.h $ $Rev: 979659 $")
#endif
