/*
 * Copyright (c) 2023, BlackBerry Limited.
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
 */

#ifndef __INIT_INTRINFO_H
#define __INIT_INTRINFO_H

extern struct callout_rtn interrupt_id_imx_msi;
extern struct callout_rtn interrupt_eoi_imx_msi;
extern struct callout_rtn interrupt_mask_imx_msi;
extern struct callout_rtn interrupt_unmask_imx_msi;

extern struct callout_rtn   interrupt_id_imx_gpio_low;
extern struct callout_rtn   interrupt_eoi_imx_gpio_low;
extern struct callout_rtn   interrupt_mask_imx_gpio_low;
extern struct callout_rtn   interrupt_unmask_imx_gpio_low;

extern struct callout_rtn   interrupt_id_imx_gpio_high;
extern struct callout_rtn   interrupt_eoi_imx_gpio_high;
extern struct callout_rtn   interrupt_mask_imx_gpio_high;
extern struct callout_rtn   interrupt_unmask_imx_gpio_high;

extern struct callout_rtn interrupt_id_imx_irqsteer;
extern struct callout_rtn interrupt_eoi_imx_irqsteer;
extern struct callout_rtn interrupt_mask_imx_irqsteer;
extern struct callout_rtn interrupt_unmask_imx_irqsteer;

#endif /* __INIT_INTRINFO_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_intrinfo.h $ $Rev: 989175 $")
#endif
