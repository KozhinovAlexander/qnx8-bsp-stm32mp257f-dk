/*
 * Copyright (c) 2016, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017-2018, NXP.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef IMX_GPIO_H_
#define IMX_GPIO_H_


/* GPIO Registers, offset from base address */
#define IMX_GPIO_DR                             0x00
#define IMX_GPIO_GDIR                           0x04
#define IMX_GPIO_PSR                            0x08

/* GPIO interrupt configuration registers */
#define IMX_GPIO_ICR1                           0x0C
#define IMX_GPIO_ICR2                           0x10
/* IMX_GPIO_ICRx bit-fields */
#define IMX_GPIO_ICRx(index)                    ((IMX_GPIO_ICR1) + (((index) > 15) ? 4 : 0))
#define IMX_GPIO_ICRx_ICRx(index, v)            ((v) << (2 * ((index) - (((index) > 15) ? 16 : 0))))
#define IMX_GPIO_ICRx_ICRx_BV_LOW_LEVEL         0
#define IMX_GPIO_ICRx_ICRx_BV_HIGH_LEVEL        1
#define IMX_GPIO_ICRx_ICRx_BV_RISING_EDGE       2
#define IMX_GPIO_ICRx_ICRx_BV_FALLING_EDGE      3

#define IMX_GPIO_IMR                            0x14
#define IMX_GPIO_ISR                            0x18
#define IMX_GPIO_EDR                            0x1C


#endif
