/*
 * Copyright (c) 2022-2023, BlackBerry Limited.
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

 /*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <hw/inout.h>
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_src.h>
#include <soc/nxp/imx8/mp/imx_ddrc.h>
#include "board.h"
#include "imx_ipl.h"

/**
 * i.MX IPL source file.
 *
 * @file       boards/imx8mp-evk/imx_ddr.c
 * @addtogroup ipl
 * @{
 */

#if defined(IMX_SPL_BOOT)
static void lpddr4_cfg_umctl2(void)
{
    /* Initialize DDRC registers */
    out32(0x3d400304, 0x1);
    out32(0x3d400030, 0x1);
    out32(0x3d400000, 0xa3080020);
    out32(0x3d400020, 0x1323);
    out32(0x3d400024, 0x1e84800);
    out32(0x3d400064, 0x7a017c);
    out32(0x3d400070, 0x1027f10);
    out32(0x3d400074, 0x7b0);
    out32(0x3d4000d0, 0xc00307a3);
    out32(0x3d4000d4, 0xc50000);
    out32(0x3d4000dc, 0xf4003f);
    out32(0x3d4000e0, 0x330000);
    out32(0x3d4000e8, 0x660048);
    out32(0x3d4000ec, 0x160048);
    out32(0x3d400100, 0x2028222a);
    out32(0x3d400104, 0x807bf);
    out32(0x3d40010c, 0xe0e000);
    out32(0x3d400110, 0x12040a12);
    out32(0x3d400114, 0x2050f0f);
    out32(0x3d400118, 0x1010009);
    out32(0x3d40011c, 0x501);
    out32(0x3d400130, 0x20800);
    out32(0x3d400134, 0xe100002);
    out32(0x3d400138, 0x184);
    out32(0x3d400144, 0xc80064);
    out32(0x3d400180, 0x3e8001e);
    out32(0x3d400184, 0x3207a12);
    out32(0x3d400188, 0x0);
    out32(0x3d400190, 0x49f820e);
    out32(0x3d400194, 0x80303);
    out32(0x3d4001b4, 0x1f0e);
    out32(0x3d4001a0, 0xe0400018);
    out32(0x3d4001a4, 0xdf00e4);
    out32(0x3d4001a8, 0x80000000);
    out32(0x3d4001b0, 0x11);
    out32(0x3d4001c0, 0x1);
    out32(0x3d4001c4, 0x1);
    out32(0x3d4000f4, 0xc99);
    out32(0x3d400108, 0x9121c1c);
    out32(0x3d400200, 0x16);
    out32(0x3d40020c, 0x0);
    out32(0x3d400210, 0x1f1f);
    out32(0x3d400204, 0x80808);
    out32(0x3d400214, 0x7070707);
    out32(0x3d400218, 0x68070707);
    out32(0x3d40021c, 0xf08);
    out32(0x3d400250, 0x1705);
    out32(0x3d400254, 0x2c);
    out32(0x3d40025c, 0x4000030);
    out32(0x3d400264, 0x900093e7);
    out32(0x3d40026c, 0x2005574);
    out32(0x3d400400, 0x111);
    out32(0x3d400404, 0x72ff);
    out32(0x3d400408, 0x72ff);
    out32(0x3d400494, 0x2100e07);
    out32(0x3d400498, 0x620096);
    out32(0x3d40049c, 0x1100e07);
    out32(0x3d4004a0, 0xc8012c);
    out32(0x3d402020, 0x1021);
    out32(0x3d402024, 0x30d400);
    out32(0x3d402050, 0x20d000);
    out32(0x3d402064, 0xc0026);
    out32(0x3d4020dc, 0x840000);
    out32(0x3d4020e0, 0x330000);
    out32(0x3d4020e8, 0x660048);
    out32(0x3d4020ec, 0x160048);
    out32(0x3d402100, 0xa040305);
    out32(0x3d402104, 0x30407);
    out32(0x3d402108, 0x203060b);
    out32(0x3d40210c, 0x505000);
    out32(0x3d402110, 0x2040202);
    out32(0x3d402114, 0x2030202);
    out32(0x3d402118, 0x1010004);
    out32(0x3d40211c, 0x301);
    out32(0x3d402130, 0x20300);
    out32(0x3d402134, 0xa100002);
    out32(0x3d402138, 0x27);
    out32(0x3d402144, 0x14000a);
    out32(0x3d402180, 0x640004);
    out32(0x3d402190, 0x3818200);
    out32(0x3d402194, 0x80303);
    out32(0x3d4021b4, 0x100);
    out32(0x3d4020f4, 0xc99);
    out32(0x3d403020, 0x1021);
    out32(0x3d403024, 0xc3500);
    out32(0x3d403050, 0x20d000);
    out32(0x3d403064, 0x3000a);
    out32(0x3d4030dc, 0x840000);
    out32(0x3d4030e0, 0x330000);
    out32(0x3d4030e8, 0x660048);
    out32(0x3d4030ec, 0x160048);
    out32(0x3d403100, 0xa010102);
    out32(0x3d403104, 0x30404);
    out32(0x3d403108, 0x203060b);
    out32(0x3d40310c, 0x505000);
    out32(0x3d403110, 0x2040202);
    out32(0x3d403114, 0x2030202);
    out32(0x3d403118, 0x1010004);
    out32(0x3d40311c, 0x301);
    out32(0x3d403130, 0x20300);
    out32(0x3d403134, 0xa100002);
    out32(0x3d403138, 0xa);
    out32(0x3d403144, 0x50003);
    out32(0x3d403180, 0x190004);
    out32(0x3d403190, 0x3818200);
    out32(0x3d403194, 0x80303);
    out32(0x3d4031b4, 0x100);
    out32(0x3d4030f4, 0xc99);

    /* Default boot point */
    out32(0x3d400028, 0x0);
}

void imx_init_ddr(void)
{
    uint32_t val, freq;

    /* Reset DDR controller */
    out32(IMX_SRC_BASE + IMX_SRC_DDRC_RCR, 0x8F00001F);
    out32(IMX_SRC_BASE + IMX_SRC_DDRC_RCR, 0x8F00000F);

    /* Change the clock source of dram_apb_clk_root: SYSTEM_PLL1 / 4 = 200MHz */
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                         IMX_CCM_TARGET_ROOT_MUX_VALUE(4) |
                                                                         IMX_CCM_TARGET_ROOT_PRE_PODF(4)));

    /* Disable ISO */
    out32(IMX_GPC_BASE + 0x00EC, 0x0000FFFF);
    val = (in32(IMX_GPC_BASE + 0x00F8) | (0x01 << 5));
    out32(IMX_GPC_BASE + 0x00F8, val);

    ddrphy_init_set_dfi_clk(4000);

    /* Release DDRC reset */
    out32(IMX_SRC_BASE + IMX_SRC_DDRC_RCR, 0x8F000006);

    /* Configure uMCTL2's registers */
    lpddr4_cfg_umctl2();

    /* Deassert all reset */
    out32(IMX_SRC_BASE + IMX_SRC_DDRC_RCR, 0x8F000004);
    out32(IMX_SRC_BASE + IMX_SRC_DDRC_RCR, 0x8F000000);

    /* Disable auto refresh */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DBG1, 0x00000000);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_RFSHCTL3, 0x0000001);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_PWRCTL, 0x000000A0);

    /* Set LPDDR4 mode */
    out32(IMX_DDR_SS_GRP0_BASE, 0x0001);

    /* Determine the initial boot frequency */
    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_MSTR);
    freq = (in32(IMX_DDR_CTL_BASE + IMX_DDRC_MSTR2) & 0x03);
    freq = (val & (0x1 << 29)) ? freq : 0x0000;
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000000);

    /* Set the default boot frequency point */
    val = (in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC) & ~(0x1F << 8));
    val |= (freq << 8);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC, val);

    val = (in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC) & ~(0x01));
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC, val);

    /* Disable quasi dynamic programming */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000001);
    while ((in32(IMX_DDR_CTL_BASE + IMX_DDRC_SWSTAT) & 0x01) == 0)
    {
    }

    /* Configure DDR PHY's registers */
    lpddr4_cfg_phy();

    while ((in32(IMX_DDR_PHY_BASE + IMX_DDRPHY_CAL_BUSY_0) & 0x01) != 0)
    {
    }

    /* Enable quasi dynamic programming */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000000);

    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC) | (0x01 << 5);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC, val);

    /* Disable quasi dynamic programming */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000001);
    while ((in32(IMX_DDR_CTL_BASE + IMX_DDRC_SWSTAT) & 0x01) == 0x00)
    {
    }

    /* Wait for DFI init complete to 1 */
    while ((in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFISTAT) & 0x01) == 0x00)
    {
    }

    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000000);

    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC) & ~(0x01 << 5);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC, val);

    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC) | 0x01;
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_DFIMISC, val);
    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_PWRCTL) & ~(0x01 << 5);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_PWRCTL, val);
    /* sw_done=1 */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_SWCTL, 0x00000001);
    /* Wait SWSTAT.sw_done_ack to 1 */
    while ((in32(IMX_DDR_CTL_BASE + IMX_DDRC_SWSTAT) & 0x01) == 0x00)
    {
    }
    /* Wait STAT.operating_mode([1:0] for ddr3) to normal state */
    while ((in32(IMX_DDR_CTL_BASE + IMX_DDRC_STAT) & 0x03) != 0x01)
    {
    }

    out32(IMX_DDR_CTL_BASE + IMX_DDRC_RFSHCTL3, 0x00000000);

    /* Enable port 0 */
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_PCTRL_0, 0x00000001);

    /* Enable selfrefresh */
    val = in32(IMX_DDR_CTL_BASE + IMX_DDRC_PWRCTL) | (0x01 << 0);
    out32(IMX_DDR_CTL_BASE + IMX_DDRC_PWRCTL, val);
}
#endif /* IMX_SPL_BOOT */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_ddr.c $ $Rev: 980064 $")
#endif
