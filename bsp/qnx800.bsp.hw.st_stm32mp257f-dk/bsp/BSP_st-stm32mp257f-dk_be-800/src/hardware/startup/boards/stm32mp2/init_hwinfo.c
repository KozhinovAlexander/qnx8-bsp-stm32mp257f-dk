/*
 * Copyright 2026, Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
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
 * init_hwinfo.c
 * Tell syspage about our HW configuration
 */

#include <startup.h>
#include <drvr/hwinfo.h>  /* For hwi support routines in libdrvr */
#include "board.h"
#include "stm32mp2_startup.h"

/**
 * STM32MP2 startup source file.
 *
 * @file       init_hwinfo.c
 * @addtogroup startup
 * @{
 */

static char serial[128];

const char* stm32mp2_get_board_serial()
{
  ksprintf(serial, "%L", 0x123456789ABCDEF); // TODO: implement reading the serial number from the board
  return serial;
}

 /**
  * Initialize the board type information in the syspage.
  * This function uses "Device electronic signature" (RM0457 Rev 5 5837/5881)
  */
void stm32mp2_init_hwinfo(void)
{
  // board_rev = get_board_revision();
  add_typed_string(_CS_MACHINE, "STM32MP257F-DK Board (MB1605 Var1.0 Rev.C-01)");  /* Name of the hardware type on which the system is running */
  add_typed_string(_CS_HW_PROVIDER, "STMicroelectronics");
  add_typed_string(_CS_ARCHITECTURE, "--- dummy arch ---");  /* Name of the instructions set architechure */
  add_typed_string(_CS_HW_SERIAL, stm32mp2_get_board_serial());  /* A serial number assiciated with the hardware */

  // add_typed_string(_CS_HOSTNAME, "--- dummy value ---");  /* Name of this node within the communications network */
  // add_typed_string(_CS_RELEASE, "--- dummy value ---");  /* Current release level of this implementation */
  // add_typed_string(_CS_VERSION, "--- dummy value ---");  /* Current version of this release */
  // add_typed_string(_CS_HW_PROVIDER, "--- dummy value ---");  /* The name of the hardware manufacturers */
  // add_typed_string(_CS_SYSNAME, "--- dummy value ---");  /* Name of this implementation of the operating system */
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/init_hwinfo.c $ $Rev: 985114 $")
#endif
