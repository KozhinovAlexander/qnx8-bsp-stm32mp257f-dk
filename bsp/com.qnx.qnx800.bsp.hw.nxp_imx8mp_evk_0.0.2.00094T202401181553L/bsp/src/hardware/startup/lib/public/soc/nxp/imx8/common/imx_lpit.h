/*
 * Copyright 2023, BlackBerry Limited. All rights reserved.
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

#ifndef IMX_LPIT_H_
#define IMX_LPIT_H_

/* LPIT registers, offset from base address */
#define IMX_LPIT_VERID                   0x00
#define IMX_LPIT_PARAM                   0x04
#define IMX_LPIT_MCR                     0x08
#define IMX_LPIT_MSR                     0x0C
#define IMX_LPIT_MIER                    0x10
#define IMX_LPIT_SETTEN                  0x14
#define IMX_LPIT_CLRTEN                  0x18
#define IMX_LPIT_TVAL(index)             (0x20 + ((index) * 0x10))
#define IMX_LPIT_CVAL(index)             (0x24 + ((index) * 0x10))
#define IMX_LPIT_TCTRL(index)            (0x28 + ((index) * 0x10))

/* MCR Register*/
#define IMX_LPIT_MCR_SWRST_MASK          (1 << 1)
#define IMX_LPIT_MCR_MCEN_MASK           (1 << 0)

/* SETTEN Register */
#define IMX_LPIT_SETTEN_EN3_MASK         (1 << 3)
#define IMX_LPIT_SETTEN_EN2_MASK         (1 << 2)
#define IMX_LPIT_SETTEN_EN1_MASK         (1 << 1)
#define IMX_LPIT_SETTEN_EN0_MASK         (1 << 0)

#endif /* IMX_LPIT_H_ */

