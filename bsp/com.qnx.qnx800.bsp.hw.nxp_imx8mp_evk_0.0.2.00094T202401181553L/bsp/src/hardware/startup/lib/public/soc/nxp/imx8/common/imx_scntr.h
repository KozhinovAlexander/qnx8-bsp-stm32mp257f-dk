/*
 * $QNXLicenseC:
 * Copyright 2019 NXP
 * Copyright 2020, QNX Software Systems.
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

#ifndef IMX_SCNTR_H_
#define IMX_SCNTR_H_

/*
 * System Counter Registers, offset from base address
 */
#define IMX_SCNTR_CR_OFF               0x00
#define IMX_SCNTR_FID0_OFF             0x20
#define IMX_SCNTR_FID1_OFF             0x24

/* WDOG_WCR bit fields */
#define IMX_SCNTR_CR_ENABLE            (0x01 << 0)
#define IMX_SCNTR_CR_HDBG              (0x01 << 1)
#define IMX_SCNTR_CR_FREQ0             (0x01 << 8)
#define IMX_SCNTR_CR_FREQ1             (0x01 << 9)

#endif /* IMX_SCNTR_H_ */
