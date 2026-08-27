/*
 * Copyright (c) 2017, NXP.
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

#ifndef IMX_INTMUX_H_
#define IMX_INTMUX_H_

/* INTMUX registers, offset from base address */

#define IMX_INTMUX_CHAN(n)             (64 * (n)) /* Channel offset from base address */

#define IMX_INTMUX_CSR                 0x00     /* Control Status Register */
#define IMX_INTMUX_VEC                 0x04     /* Vector Number Register */
#define IMX_INTMUX_IER                 0x10     /* Interrupt Enable Register */
#define IMX_INTMUX_IPR                 0x20     /* Interrupt Pending Register */


#endif /* IMX_INTMUX_H_ */
