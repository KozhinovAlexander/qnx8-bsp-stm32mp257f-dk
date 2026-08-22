/*
 * $QNXLicenseC:
 * Copyright 2023 BlackBerry Limited.
 * Copyright 2023 NXP
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

/* Steer interrupt indexing start at regN bit 0 and finish
 * at reg0 bit 31. HDMI steer supports up to 64 interrupts
 * but only 15 is used. So it is necessary handle the regN
 * registers where N = 1. Also this register map is only
 * valid for HDMI now. If DSP steer needed the code related
 * to steer support needs to be updated and unified to handle
 * both cases. */
#define IMX_IRQ_STEER_STAT                     IMX_IRQ_STEER_CHN_STATUS1
#define IMX_IRQ_STEER_MASK                     IMX_IRQ_STEER_CHN_MASK1

/* IRQSTEER registers */
#define IMX_IRQ_STEER_CHN_MASK0                0x4
#define IMX_IRQ_STEER_CHN_MASK1                0x8
#define IMX_IRQ_STEER_CHN_SET0                 0xC
#define IMX_IRQ_STEER_CHN_SET1                 0x10
#define IMX_IRQ_STEER_CHN_STATUS0              0x14
#define IMX_IRQ_STEER_CHN_STATUS1              0x18
#define IMX_IRQ_STEER_CHN_MINTDIS              0x1C
#define IMX_IRQ_STEER_CHN_MSTRSTAT             0x20
