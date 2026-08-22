/*
 * Copyright (c) 2022 BlackBerry Limited.
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


#ifndef MP_EVK_CPU_H
#define MP_EVK_CPU_H

#include "imx_startup.h"
#include "board.h"


/* HW info */
void imx_init_hwinfo(imx_startup_data_t *startup_data);

/* Ethernet function prototype */
#if IMX_ENET_GET_MAC_ENABLED
void imx_init_enet_mac_addr(void);
#endif

#if (IMX_CHECK_RESET == 1)
    void imx_check_reset_source(void);
#endif


#endif /* MP_EVK_CPU_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/mp_evk_cpu.h $ $Rev: 965915 $")
#endif
