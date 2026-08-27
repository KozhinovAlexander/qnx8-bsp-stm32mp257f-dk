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

/*
 * Copyright 2019 NXP
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
#include "ipl.h"
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_ddrc.h>
#include "board.h"
#include "imx_ipl.h"

#define IMEM_LEN                    32768
#define DMEM_LEN                    16384
#define IMEM_2D_OFFSET              49152

#define IMEM_OFFSET_ADDR            0x00050000
#define DMEM_OFFSET_ADDR            0x00054000
#define DDR_TRAIN_CODE_BASE_ADDR    IMX_DDR_PHY_BASE

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

enum fw_type_t {
    IMX_FW_1D_IMAGE,
    IMX_FW_2D_IMAGE,
};

struct dram_cfg_param {
    unsigned int reg;
    unsigned int val;
};

struct dram_fsp_msg {
    unsigned int drate;
    enum fw_type_t fw_type;
    struct dram_cfg_param *fsp_cfg;
    unsigned int fsp_cfg_num;
};

#if defined(IMX_SPL_BOOT)
/* PHY Initialize Configuration */
static struct dram_cfg_param lpddr4_ddrphy_cfg[] = {
    { .reg = 0x100a0, .val = 0x0 },
    { .reg = 0x100a1, .val = 0x1 },
    { .reg = 0x100a2, .val = 0x2 },
    { .reg = 0x100a3, .val = 0x3 },
    { .reg = 0x100a4, .val = 0x4 },
    { .reg = 0x100a5, .val = 0x5 },
    { .reg = 0x100a6, .val = 0x6 },
    { .reg = 0x100a7, .val = 0x7 },
    { .reg = 0x110a0, .val = 0x0 },
    { .reg = 0x110a1, .val = 0x1 },
    { .reg = 0x110a2, .val = 0x3 },
    { .reg = 0x110a3, .val = 0x4 },
    { .reg = 0x110a4, .val = 0x5 },
    { .reg = 0x110a5, .val = 0x2 },
    { .reg = 0x110a6, .val = 0x7 },
    { .reg = 0x110a7, .val = 0x6 },
    { .reg = 0x120a0, .val = 0x0 },
    { .reg = 0x120a1, .val = 0x1 },
    { .reg = 0x120a2, .val = 0x3 },
    { .reg = 0x120a3, .val = 0x2 },
    { .reg = 0x120a4, .val = 0x5 },
    { .reg = 0x120a5, .val = 0x4 },
    { .reg = 0x120a6, .val = 0x7 },
    { .reg = 0x120a7, .val = 0x6 },
    { .reg = 0x130a0, .val = 0x0 },
    { .reg = 0x130a1, .val = 0x1 },
    { .reg = 0x130a2, .val = 0x2 },
    { .reg = 0x130a3, .val = 0x3 },
    { .reg = 0x130a4, .val = 0x4 },
    { .reg = 0x130a5, .val = 0x5 },
    { .reg = 0x130a6, .val = 0x6 },
    { .reg = 0x130a7, .val = 0x7 },
    { .reg = 0x1005f, .val = 0x1ff },
    { .reg = 0x1015f, .val = 0x1ff },
    { .reg = 0x1105f, .val = 0x1ff },
    { .reg = 0x1115f, .val = 0x1ff },
    { .reg = 0x1205f, .val = 0x1ff },
    { .reg = 0x1215f, .val = 0x1ff },
    { .reg = 0x1305f, .val = 0x1ff },
    { .reg = 0x1315f, .val = 0x1ff },
    { .reg = 0x11005f, .val = 0x1ff },
    { .reg = 0x11015f, .val = 0x1ff },
    { .reg = 0x11105f, .val = 0x1ff },
    { .reg = 0x11115f, .val = 0x1ff },
    { .reg = 0x11205f, .val = 0x1ff },
    { .reg = 0x11215f, .val = 0x1ff },
    { .reg = 0x11305f, .val = 0x1ff },
    { .reg = 0x11315f, .val = 0x1ff },
    { .reg = 0x21005f, .val = 0x1ff },
    { .reg = 0x21015f, .val = 0x1ff },
    { .reg = 0x21105f, .val = 0x1ff },
    { .reg = 0x21115f, .val = 0x1ff },
    { .reg = 0x21205f, .val = 0x1ff },
    { .reg = 0x21215f, .val = 0x1ff },
    { .reg = 0x21305f, .val = 0x1ff },
    { .reg = 0x21315f, .val = 0x1ff },
    { .reg = 0x55, .val = 0x1ff },
    { .reg = 0x1055, .val = 0x1ff },
    { .reg = 0x2055, .val = 0x1ff },
    { .reg = 0x3055, .val = 0x1ff },
    { .reg = 0x4055, .val = 0x1ff },
    { .reg = 0x5055, .val = 0x1ff },
    { .reg = 0x6055, .val = 0x1ff },
    { .reg = 0x7055, .val = 0x1ff },
    { .reg = 0x8055, .val = 0x1ff },
    { .reg = 0x9055, .val = 0x1ff },
    { .reg = 0x200c5, .val = 0x18 },
    { .reg = 0x1200c5, .val = 0x7 },
    { .reg = 0x2200c5, .val = 0x7 },
    { .reg = 0x2002e, .val = 0x2 },
    { .reg = 0x12002e, .val = 0x2 },
    { .reg = 0x22002e, .val = 0x2 },
    { .reg = 0x90204, .val = 0x0 },
    { .reg = 0x190204, .val = 0x0 },
    { .reg = 0x290204, .val = 0x0 },
    { .reg = 0x20024, .val = 0x1e3 },
    { .reg = 0x2003a, .val = 0x2 },
    { .reg = 0x120024, .val = 0x1e3 },
    { .reg = 0x2003a, .val = 0x2 },
    { .reg = 0x220024, .val = 0x1e3 },
    { .reg = 0x2003a, .val = 0x2 },
    { .reg = 0x20056, .val = 0x3 },
    { .reg = 0x120056, .val = 0x3 },
    { .reg = 0x220056, .val = 0x3 },
    { .reg = 0x1004d, .val = 0xe00 },
    { .reg = 0x1014d, .val = 0xe00 },
    { .reg = 0x1104d, .val = 0xe00 },
    { .reg = 0x1114d, .val = 0xe00 },
    { .reg = 0x1204d, .val = 0xe00 },
    { .reg = 0x1214d, .val = 0xe00 },
    { .reg = 0x1304d, .val = 0xe00 },
    { .reg = 0x1314d, .val = 0xe00 },
    { .reg = 0x11004d, .val = 0xe00 },
    { .reg = 0x11014d, .val = 0xe00 },
    { .reg = 0x11104d, .val = 0xe00 },
    { .reg = 0x11114d, .val = 0xe00 },
    { .reg = 0x11204d, .val = 0xe00 },
    { .reg = 0x11214d, .val = 0xe00 },
    { .reg = 0x11304d, .val = 0xe00 },
    { .reg = 0x11314d, .val = 0xe00 },
    { .reg = 0x21004d, .val = 0xe00 },
    { .reg = 0x21014d, .val = 0xe00 },
    { .reg = 0x21104d, .val = 0xe00 },
    { .reg = 0x21114d, .val = 0xe00 },
    { .reg = 0x21204d, .val = 0xe00 },
    { .reg = 0x21214d, .val = 0xe00 },
    { .reg = 0x21304d, .val = 0xe00 },
    { .reg = 0x21314d, .val = 0xe00 },
    { .reg = 0x10049, .val = 0xeba },
    { .reg = 0x10149, .val = 0xeba },
    { .reg = 0x11049, .val = 0xeba },
    { .reg = 0x11149, .val = 0xeba },
    { .reg = 0x12049, .val = 0xeba },
    { .reg = 0x12149, .val = 0xeba },
    { .reg = 0x13049, .val = 0xeba },
    { .reg = 0x13149, .val = 0xeba },
    { .reg = 0x110049, .val = 0xeba },
    { .reg = 0x110149, .val = 0xeba },
    { .reg = 0x111049, .val = 0xeba },
    { .reg = 0x111149, .val = 0xeba },
    { .reg = 0x112049, .val = 0xeba },
    { .reg = 0x112149, .val = 0xeba },
    { .reg = 0x113049, .val = 0xeba },
    { .reg = 0x113149, .val = 0xeba },
    { .reg = 0x210049, .val = 0xeba },
    { .reg = 0x210149, .val = 0xeba },
    { .reg = 0x211049, .val = 0xeba },
    { .reg = 0x211149, .val = 0xeba },
    { .reg = 0x212049, .val = 0xeba },
    { .reg = 0x212149, .val = 0xeba },
    { .reg = 0x213049, .val = 0xeba },
    { .reg = 0x213149, .val = 0xeba },
    { .reg = 0x43, .val = 0x63 },
    { .reg = 0x1043, .val = 0x63 },
    { .reg = 0x2043, .val = 0x63 },
    { .reg = 0x3043, .val = 0x63 },
    { .reg = 0x4043, .val = 0x63 },
    { .reg = 0x5043, .val = 0x63 },
    { .reg = 0x6043, .val = 0x63 },
    { .reg = 0x7043, .val = 0x63 },
    { .reg = 0x8043, .val = 0x63 },
    { .reg = 0x9043, .val = 0x63 },
    { .reg = 0x20018, .val = 0x3 },
    { .reg = 0x20075, .val = 0x4 },
    { .reg = 0x20050, .val = 0x0 },
    { .reg = 0x20008, .val = 0x3e8 },
    { .reg = 0x120008, .val = 0x64 },
    { .reg = 0x220008, .val = 0x19 },
    { .reg = 0x20088, .val = 0x9 },
    { .reg = 0x200b2, .val = 0x104 },
    { .reg = 0x10043, .val = 0x5a1 },
    { .reg = 0x10143, .val = 0x5a1 },
    { .reg = 0x11043, .val = 0x5a1 },
    { .reg = 0x11143, .val = 0x5a1 },
    { .reg = 0x12043, .val = 0x5a1 },
    { .reg = 0x12143, .val = 0x5a1 },
    { .reg = 0x13043, .val = 0x5a1 },
    { .reg = 0x13143, .val = 0x5a1 },
    { .reg = 0x1200b2, .val = 0x104 },
    { .reg = 0x110043, .val = 0x5a1 },
    { .reg = 0x110143, .val = 0x5a1 },
    { .reg = 0x111043, .val = 0x5a1 },
    { .reg = 0x111143, .val = 0x5a1 },
    { .reg = 0x112043, .val = 0x5a1 },
    { .reg = 0x112143, .val = 0x5a1 },
    { .reg = 0x113043, .val = 0x5a1 },
    { .reg = 0x113143, .val = 0x5a1 },
    { .reg = 0x2200b2, .val = 0x104 },
    { .reg = 0x210043, .val = 0x5a1 },
    { .reg = 0x210143, .val = 0x5a1 },
    { .reg = 0x211043, .val = 0x5a1 },
    { .reg = 0x211143, .val = 0x5a1 },
    { .reg = 0x212043, .val = 0x5a1 },
    { .reg = 0x212143, .val = 0x5a1 },
    { .reg = 0x213043, .val = 0x5a1 },
    { .reg = 0x213143, .val = 0x5a1 },
    { .reg = 0x200fa, .val = 0x1 },
    { .reg = 0x1200fa, .val = 0x1 },
    { .reg = 0x2200fa, .val = 0x1 },
    { .reg = 0x20019, .val = 0x1 },
    { .reg = 0x120019, .val = 0x1 },
    { .reg = 0x220019, .val = 0x1 },
    { .reg = 0x200f0, .val = 0x660 },
    { .reg = 0x200f1, .val = 0x0 },
    { .reg = 0x200f2, .val = 0x4444 },
    { .reg = 0x200f3, .val = 0x8888 },
    { .reg = 0x200f4, .val = 0x5665 },
    { .reg = 0x200f5, .val = 0x0 },
    { .reg = 0x200f6, .val = 0x0 },
    { .reg = 0x200f7, .val = 0xf000 },
    { .reg = 0x20025, .val = 0x0 },
    { .reg = 0x2002d, .val = 0x0 },
    { .reg = 0x12002d, .val = 0x0 },
    { .reg = 0x22002d, .val = 0x0 },
    { .reg = 0x2007d, .val = 0x212 },
    { .reg = 0x12007d, .val = 0x212 },
    { .reg = 0x22007d, .val = 0x212 },
    { .reg = 0x2007c, .val = 0x61 },
    { .reg = 0x12007c, .val = 0x61 },
    { .reg = 0x22007c, .val = 0x61 },
    { .reg = 0x1004a, .val = 0x500 },
    { .reg = 0x1104a, .val = 0x500 },
    { .reg = 0x1204a, .val = 0x500 },
    { .reg = 0x1304a, .val = 0x500 },
    { .reg = 0x2002c, .val = 0x0 },
};

/* P0 message block paremeter for training firmware */
static struct dram_cfg_param lpddr4_fsp0_cfg[] = {
    { .reg = 0xd0000, .val = 0x0 },
    { .reg = 0x54003, .val = 0xfa0 },
    { .reg = 0x54004, .val = 0x2 },
    { .reg = 0x54005, .val = 0x2228 },
    { .reg = 0x54006, .val = 0x14 },
    { .reg = 0x54008, .val = 0x131f },
    { .reg = 0x54009, .val = 0xc8 },
    { .reg = 0x5400b, .val = 0x2 },
    { .reg = 0x5400f, .val = 0x100 },
    { .reg = 0x54012, .val = 0x310 },
    { .reg = 0x54019, .val = 0x3ff4 },
    { .reg = 0x5401a, .val = 0x33 },
    { .reg = 0x5401b, .val = 0x4866 },
    { .reg = 0x5401c, .val = 0x4800 },
    { .reg = 0x5401e, .val = 0x16 },
    { .reg = 0x5401f, .val = 0x3ff4 },
    { .reg = 0x54020, .val = 0x33 },
    { .reg = 0x54021, .val = 0x4866 },
    { .reg = 0x54022, .val = 0x4800 },
    { .reg = 0x54024, .val = 0x16 },
    { .reg = 0x5402b, .val = 0x1000 },
    { .reg = 0x5402c, .val = 0x3 },
    { .reg = 0x54032, .val = 0xf400 },
    { .reg = 0x54033, .val = 0x333f },
    { .reg = 0x54034, .val = 0x6600 },
    { .reg = 0x54035, .val = 0x48 },
    { .reg = 0x54036, .val = 0x48 },
    { .reg = 0x54037, .val = 0x1600 },
    { .reg = 0x54038, .val = 0xf400 },
    { .reg = 0x54039, .val = 0x333f },
    { .reg = 0x5403a, .val = 0x6600 },
    { .reg = 0x5403b, .val = 0x48 },
    { .reg = 0x5403c, .val = 0x48 },
    { .reg = 0x5403d, .val = 0x1600 },
    { .reg = 0xd0000, .val = 0x1 },
};

/* P0 2D message block paremeter for training firmware */
static struct dram_cfg_param lpddr4_fsp0_2d_cfg[] = {
    { .reg = 0xd0000, .val = 0x0 },
    { .reg = 0x54003, .val = 0xfa0 },
    { .reg = 0x54004, .val = 0x2 },
    { .reg = 0x54005, .val = 0x2228 },
    { .reg = 0x54006, .val = 0x14 },
    { .reg = 0x54008, .val = 0x61 },
    { .reg = 0x54009, .val = 0xc8 },
    { .reg = 0x5400b, .val = 0x2 },
    { .reg = 0x5400f, .val = 0x100 },
    { .reg = 0x54010, .val = 0x1f7f },
    { .reg = 0x54012, .val = 0x310 },
    { .reg = 0x54019, .val = 0x3ff4 },
    { .reg = 0x5401a, .val = 0x33 },
    { .reg = 0x5401b, .val = 0x4866 },
    { .reg = 0x5401c, .val = 0x4800 },
    { .reg = 0x5401e, .val = 0x16 },
    { .reg = 0x5401f, .val = 0x3ff4 },
    { .reg = 0x54020, .val = 0x33 },
    { .reg = 0x54021, .val = 0x4866 },
    { .reg = 0x54022, .val = 0x4800 },
    { .reg = 0x54024, .val = 0x16 },
    { .reg = 0x5402b, .val = 0x1000 },
    { .reg = 0x5402c, .val = 0x3 },
    { .reg = 0x54032, .val = 0xf400 },
    { .reg = 0x54033, .val = 0x333f },
    { .reg = 0x54034, .val = 0x6600 },
    { .reg = 0x54035, .val = 0x48 },
    { .reg = 0x54036, .val = 0x48 },
    { .reg = 0x54037, .val = 0x1600 },
    { .reg = 0x54038, .val = 0xf400 },
    { .reg = 0x54039, .val = 0x333f },
    { .reg = 0x5403a, .val = 0x6600 },
    { .reg = 0x5403b, .val = 0x48 },
    { .reg = 0x5403c, .val = 0x48 },
    { .reg = 0x5403d, .val = 0x1600 },
    { .reg = 0xd0000, .val = 0x1 },
};

/* P1 message block paremeter for training firmware */
static struct dram_cfg_param lpddr4_fsp1_cfg[] = {
    { .reg = 0xd0000, .val = 0x0 },
    { .reg = 0x54002, .val = 0x101 },
    { .reg = 0x54003, .val = 0x190 },
    { .reg = 0x54004, .val = 0x2 },
    { .reg = 0x54005, .val = 0x2228 },
    { .reg = 0x54006, .val = 0x14 },
    { .reg = 0x54008, .val = 0x121f },
    { .reg = 0x54009, .val = 0xc8 },
    { .reg = 0x5400b, .val = 0x2 },
    { .reg = 0x5400f, .val = 0x100 },
    { .reg = 0x54012, .val = 0x310 },
    { .reg = 0x54019, .val = 0x84 },
    { .reg = 0x5401a, .val = 0x33 },
    { .reg = 0x5401b, .val = 0x4866 },
    { .reg = 0x5401c, .val = 0x4800 },
    { .reg = 0x5401e, .val = 0x16 },
    { .reg = 0x5401f, .val = 0x84 },
    { .reg = 0x54020, .val = 0x33 },
    { .reg = 0x54021, .val = 0x4866 },
    { .reg = 0x54022, .val = 0x4800 },
    { .reg = 0x54024, .val = 0x16 },
    { .reg = 0x5402b, .val = 0x1000 },
    { .reg = 0x5402c, .val = 0x3 },
    { .reg = 0x54032, .val = 0x8400 },
    { .reg = 0x54033, .val = 0x3300 },
    { .reg = 0x54034, .val = 0x6600 },
    { .reg = 0x54035, .val = 0x48 },
    { .reg = 0x54036, .val = 0x48 },
    { .reg = 0x54037, .val = 0x1600 },
    { .reg = 0x54038, .val = 0x8400 },
    { .reg = 0x54039, .val = 0x3300 },
    { .reg = 0x5403a, .val = 0x6600 },
    { .reg = 0x5403b, .val = 0x48 },
    { .reg = 0x5403c, .val = 0x48 },
    { .reg = 0x5403d, .val = 0x1600 },
    { .reg = 0xd0000, .val = 0x1 },
};

/* P1 message block paremeter for training firmware */
static struct dram_cfg_param lpddr4_fsp2_cfg[] = {
    { .reg = 0xd0000, .val = 0x0 },
    { .reg = 0x54002, .val = 0x102 },
    { .reg = 0x54003, .val = 0x64 },
    { .reg = 0x54004, .val = 0x2 },
    { .reg = 0x54005, .val = 0x2228 },
    { .reg = 0x54006, .val = 0x14 },
    { .reg = 0x54008, .val = 0x121f },
    { .reg = 0x54009, .val = 0xc8 },
    { .reg = 0x5400b, .val = 0x2 },
    { .reg = 0x5400f, .val = 0x100 },
    { .reg = 0x54012, .val = 0x310 },
    { .reg = 0x54019, .val = 0x84 },
    { .reg = 0x5401a, .val = 0x33 },
    { .reg = 0x5401b, .val = 0x4866 },
    { .reg = 0x5401c, .val = 0x4800 },
    { .reg = 0x5401e, .val = 0x16 },
    { .reg = 0x5401f, .val = 0x84 },
    { .reg = 0x54020, .val = 0x33 },
    { .reg = 0x54021, .val = 0x4866 },
    { .reg = 0x54022, .val = 0x4800 },
    { .reg = 0x54024, .val = 0x16 },
    { .reg = 0x5402b, .val = 0x1000 },
    { .reg = 0x5402c, .val = 0x3 },
    { .reg = 0x54032, .val = 0x8400 },
    { .reg = 0x54033, .val = 0x3300 },
    { .reg = 0x54034, .val = 0x6600 },
    { .reg = 0x54035, .val = 0x48 },
    { .reg = 0x54036, .val = 0x48 },
    { .reg = 0x54037, .val = 0x1600 },
    { .reg = 0x54038, .val = 0x8400 },
    { .reg = 0x54039, .val = 0x3300 },
    { .reg = 0x5403a, .val = 0x6600 },
    { .reg = 0x5403b, .val = 0x48 },
    { .reg = 0x5403c, .val = 0x48 },
    { .reg = 0x5403d, .val = 0x1600 },
    { .reg = 0xd0000, .val = 0x1 },
};

/* DRAM PHY init engine image */
static struct dram_cfg_param lpddr4_phy_pie[] = {
    { .reg = 0xd0000, .val = 0x0 },
    { .reg = 0x90000, .val = 0x10 },
    { .reg = 0x90001, .val = 0x400 },
    { .reg = 0x90002, .val = 0x10e },
    { .reg = 0x90003, .val = 0x0 },
    { .reg = 0x90004, .val = 0x0 },
    { .reg = 0x90005, .val = 0x8 },
    { .reg = 0x90029, .val = 0xb },
    { .reg = 0x9002a, .val = 0x480 },
    { .reg = 0x9002b, .val = 0x109 },
    { .reg = 0x9002c, .val = 0x8 },
    { .reg = 0x9002d, .val = 0x448 },
    { .reg = 0x9002e, .val = 0x139 },
    { .reg = 0x9002f, .val = 0x8 },
    { .reg = 0x90030, .val = 0x478 },
    { .reg = 0x90031, .val = 0x109 },
    { .reg = 0x90032, .val = 0x0 },
    { .reg = 0x90033, .val = 0xe8 },
    { .reg = 0x90034, .val = 0x109 },
    { .reg = 0x90035, .val = 0x2 },
    { .reg = 0x90036, .val = 0x10 },
    { .reg = 0x90037, .val = 0x139 },
    { .reg = 0x90038, .val = 0xb },
    { .reg = 0x90039, .val = 0x7c0 },
    { .reg = 0x9003a, .val = 0x139 },
    { .reg = 0x9003b, .val = 0x44 },
    { .reg = 0x9003c, .val = 0x633 },
    { .reg = 0x9003d, .val = 0x159 },
    { .reg = 0x9003e, .val = 0x14f },
    { .reg = 0x9003f, .val = 0x630 },
    { .reg = 0x90040, .val = 0x159 },
    { .reg = 0x90041, .val = 0x47 },
    { .reg = 0x90042, .val = 0x633 },
    { .reg = 0x90043, .val = 0x149 },
    { .reg = 0x90044, .val = 0x4f },
    { .reg = 0x90045, .val = 0x633 },
    { .reg = 0x90046, .val = 0x179 },
    { .reg = 0x90047, .val = 0x8 },
    { .reg = 0x90048, .val = 0xe0 },
    { .reg = 0x90049, .val = 0x109 },
    { .reg = 0x9004a, .val = 0x0 },
    { .reg = 0x9004b, .val = 0x7c8 },
    { .reg = 0x9004c, .val = 0x109 },
    { .reg = 0x9004d, .val = 0x0 },
    { .reg = 0x9004e, .val = 0x1 },
    { .reg = 0x9004f, .val = 0x8 },
    { .reg = 0x90050, .val = 0x0 },
    { .reg = 0x90051, .val = 0x45a },
    { .reg = 0x90052, .val = 0x9 },
    { .reg = 0x90053, .val = 0x0 },
    { .reg = 0x90054, .val = 0x448 },
    { .reg = 0x90055, .val = 0x109 },
    { .reg = 0x90056, .val = 0x40 },
    { .reg = 0x90057, .val = 0x633 },
    { .reg = 0x90058, .val = 0x179 },
    { .reg = 0x90059, .val = 0x1 },
    { .reg = 0x9005a, .val = 0x618 },
    { .reg = 0x9005b, .val = 0x109 },
    { .reg = 0x9005c, .val = 0x40c0 },
    { .reg = 0x9005d, .val = 0x633 },
    { .reg = 0x9005e, .val = 0x149 },
    { .reg = 0x9005f, .val = 0x8 },
    { .reg = 0x90060, .val = 0x4 },
    { .reg = 0x90061, .val = 0x48 },
    { .reg = 0x90062, .val = 0x4040 },
    { .reg = 0x90063, .val = 0x633 },
    { .reg = 0x90064, .val = 0x149 },
    { .reg = 0x90065, .val = 0x0 },
    { .reg = 0x90066, .val = 0x4 },
    { .reg = 0x90067, .val = 0x48 },
    { .reg = 0x90068, .val = 0x40 },
    { .reg = 0x90069, .val = 0x633 },
    { .reg = 0x9006a, .val = 0x149 },
    { .reg = 0x9006b, .val = 0x10 },
    { .reg = 0x9006c, .val = 0x4 },
    { .reg = 0x9006d, .val = 0x18 },
    { .reg = 0x9006e, .val = 0x0 },
    { .reg = 0x9006f, .val = 0x4 },
    { .reg = 0x90070, .val = 0x78 },
    { .reg = 0x90071, .val = 0x549 },
    { .reg = 0x90072, .val = 0x633 },
    { .reg = 0x90073, .val = 0x159 },
    { .reg = 0x90074, .val = 0xd49 },
    { .reg = 0x90075, .val = 0x633 },
    { .reg = 0x90076, .val = 0x159 },
    { .reg = 0x90077, .val = 0x94a },
    { .reg = 0x90078, .val = 0x633 },
    { .reg = 0x90079, .val = 0x159 },
    { .reg = 0x9007a, .val = 0x441 },
    { .reg = 0x9007b, .val = 0x633 },
    { .reg = 0x9007c, .val = 0x149 },
    { .reg = 0x9007d, .val = 0x42 },
    { .reg = 0x9007e, .val = 0x633 },
    { .reg = 0x9007f, .val = 0x149 },
    { .reg = 0x90080, .val = 0x1 },
    { .reg = 0x90081, .val = 0x633 },
    { .reg = 0x90082, .val = 0x149 },
    { .reg = 0x90083, .val = 0x0 },
    { .reg = 0x90084, .val = 0xe0 },
    { .reg = 0x90085, .val = 0x109 },
    { .reg = 0x90086, .val = 0xa },
    { .reg = 0x90087, .val = 0x10 },
    { .reg = 0x90088, .val = 0x109 },
    { .reg = 0x90089, .val = 0x9 },
    { .reg = 0x9008a, .val = 0x3c0 },
    { .reg = 0x9008b, .val = 0x149 },
    { .reg = 0x9008c, .val = 0x9 },
    { .reg = 0x9008d, .val = 0x3c0 },
    { .reg = 0x9008e, .val = 0x159 },
    { .reg = 0x9008f, .val = 0x18 },
    { .reg = 0x90090, .val = 0x10 },
    { .reg = 0x90091, .val = 0x109 },
    { .reg = 0x90092, .val = 0x0 },
    { .reg = 0x90093, .val = 0x3c0 },
    { .reg = 0x90094, .val = 0x109 },
    { .reg = 0x90095, .val = 0x18 },
    { .reg = 0x90096, .val = 0x4 },
    { .reg = 0x90097, .val = 0x48 },
    { .reg = 0x90098, .val = 0x18 },
    { .reg = 0x90099, .val = 0x4 },
    { .reg = 0x9009a, .val = 0x58 },
    { .reg = 0x9009b, .val = 0xb },
    { .reg = 0x9009c, .val = 0x10 },
    { .reg = 0x9009d, .val = 0x109 },
    { .reg = 0x9009e, .val = 0x1 },
    { .reg = 0x9009f, .val = 0x10 },
    { .reg = 0x900a0, .val = 0x109 },
    { .reg = 0x900a1, .val = 0x5 },
    { .reg = 0x900a2, .val = 0x7c0 },
    { .reg = 0x900a3, .val = 0x109 },
    { .reg = 0x40000, .val = 0x811 },
    { .reg = 0x40020, .val = 0x880 },
    { .reg = 0x40040, .val = 0x0 },
    { .reg = 0x40060, .val = 0x0 },
    { .reg = 0x40001, .val = 0x4008 },
    { .reg = 0x40021, .val = 0x83 },
    { .reg = 0x40041, .val = 0x4f },
    { .reg = 0x40061, .val = 0x0 },
    { .reg = 0x40002, .val = 0x4040 },
    { .reg = 0x40022, .val = 0x83 },
    { .reg = 0x40042, .val = 0x51 },
    { .reg = 0x40062, .val = 0x0 },
    { .reg = 0x40003, .val = 0x811 },
    { .reg = 0x40023, .val = 0x880 },
    { .reg = 0x40043, .val = 0x0 },
    { .reg = 0x40063, .val = 0x0 },
    { .reg = 0x40004, .val = 0x720 },
    { .reg = 0x40024, .val = 0xf },
    { .reg = 0x40044, .val = 0x1740 },
    { .reg = 0x40064, .val = 0x0 },
    { .reg = 0x40005, .val = 0x16 },
    { .reg = 0x40025, .val = 0x83 },
    { .reg = 0x40045, .val = 0x4b },
    { .reg = 0x40065, .val = 0x0 },
    { .reg = 0x40006, .val = 0x716 },
    { .reg = 0x40026, .val = 0xf },
    { .reg = 0x40046, .val = 0x2001 },
    { .reg = 0x40066, .val = 0x0 },
    { .reg = 0x40007, .val = 0x716 },
    { .reg = 0x40027, .val = 0xf },
    { .reg = 0x40047, .val = 0x2800 },
    { .reg = 0x40067, .val = 0x0 },
    { .reg = 0x40008, .val = 0x716 },
    { .reg = 0x40028, .val = 0xf },
    { .reg = 0x40048, .val = 0xf00 },
    { .reg = 0x40068, .val = 0x0 },
    { .reg = 0x40009, .val = 0x720 },
    { .reg = 0x40029, .val = 0xf },
    { .reg = 0x40049, .val = 0x1400 },
    { .reg = 0x40069, .val = 0x0 },
    { .reg = 0x4000a, .val = 0xe08 },
    { .reg = 0x4002a, .val = 0xc15 },
    { .reg = 0x4004a, .val = 0x0 },
    { .reg = 0x4006a, .val = 0x0 },
    { .reg = 0x4000b, .val = 0x625 },
    { .reg = 0x4002b, .val = 0x15 },
    { .reg = 0x4004b, .val = 0x0 },
    { .reg = 0x4006b, .val = 0x0 },
    { .reg = 0x4000c, .val = 0x4028 },
    { .reg = 0x4002c, .val = 0x80 },
    { .reg = 0x4004c, .val = 0x0 },
    { .reg = 0x4006c, .val = 0x0 },
    { .reg = 0x4000d, .val = 0xe08 },
    { .reg = 0x4002d, .val = 0xc1a },
    { .reg = 0x4004d, .val = 0x0 },
    { .reg = 0x4006d, .val = 0x0 },
    { .reg = 0x4000e, .val = 0x625 },
    { .reg = 0x4002e, .val = 0x1a },
    { .reg = 0x4004e, .val = 0x0 },
    { .reg = 0x4006e, .val = 0x0 },
    { .reg = 0x4000f, .val = 0x4040 },
    { .reg = 0x4002f, .val = 0x80 },
    { .reg = 0x4004f, .val = 0x0 },
    { .reg = 0x4006f, .val = 0x0 },
    { .reg = 0x40010, .val = 0x2604 },
    { .reg = 0x40030, .val = 0x15 },
    { .reg = 0x40050, .val = 0x0 },
    { .reg = 0x40070, .val = 0x0 },
    { .reg = 0x40011, .val = 0x708 },
    { .reg = 0x40031, .val = 0x5 },
    { .reg = 0x40051, .val = 0x0 },
    { .reg = 0x40071, .val = 0x2002 },
    { .reg = 0x40012, .val = 0x8 },
    { .reg = 0x40032, .val = 0x80 },
    { .reg = 0x40052, .val = 0x0 },
    { .reg = 0x40072, .val = 0x0 },
    { .reg = 0x40013, .val = 0x2604 },
    { .reg = 0x40033, .val = 0x1a },
    { .reg = 0x40053, .val = 0x0 },
    { .reg = 0x40073, .val = 0x0 },
    { .reg = 0x40014, .val = 0x708 },
    { .reg = 0x40034, .val = 0xa },
    { .reg = 0x40054, .val = 0x0 },
    { .reg = 0x40074, .val = 0x2002 },
    { .reg = 0x40015, .val = 0x4040 },
    { .reg = 0x40035, .val = 0x80 },
    { .reg = 0x40055, .val = 0x0 },
    { .reg = 0x40075, .val = 0x0 },
    { .reg = 0x40016, .val = 0x60a },
    { .reg = 0x40036, .val = 0x15 },
    { .reg = 0x40056, .val = 0x1200 },
    { .reg = 0x40076, .val = 0x0 },
    { .reg = 0x40017, .val = 0x61a },
    { .reg = 0x40037, .val = 0x15 },
    { .reg = 0x40057, .val = 0x1300 },
    { .reg = 0x40077, .val = 0x0 },
    { .reg = 0x40018, .val = 0x60a },
    { .reg = 0x40038, .val = 0x1a },
    { .reg = 0x40058, .val = 0x1200 },
    { .reg = 0x40078, .val = 0x0 },
    { .reg = 0x40019, .val = 0x642 },
    { .reg = 0x40039, .val = 0x1a },
    { .reg = 0x40059, .val = 0x1300 },
    { .reg = 0x40079, .val = 0x0 },
    { .reg = 0x4001a, .val = 0x4808 },
    { .reg = 0x4003a, .val = 0x880 },
    { .reg = 0x4005a, .val = 0x0 },
    { .reg = 0x4007a, .val = 0x0 },
    { .reg = 0x900a4, .val = 0x0 },
    { .reg = 0x900a5, .val = 0x790 },
    { .reg = 0x900a6, .val = 0x11a },
    { .reg = 0x900a7, .val = 0x8 },
    { .reg = 0x900a8, .val = 0x7aa },
    { .reg = 0x900a9, .val = 0x2a },
    { .reg = 0x900aa, .val = 0x10 },
    { .reg = 0x900ab, .val = 0x7b2 },
    { .reg = 0x900ac, .val = 0x2a },
    { .reg = 0x900ad, .val = 0x0 },
    { .reg = 0x900ae, .val = 0x7c8 },
    { .reg = 0x900af, .val = 0x109 },
    { .reg = 0x900b0, .val = 0x10 },
    { .reg = 0x900b1, .val = 0x10 },
    { .reg = 0x900b2, .val = 0x109 },
    { .reg = 0x900b3, .val = 0x10 },
    { .reg = 0x900b4, .val = 0x2a8 },
    { .reg = 0x900b5, .val = 0x129 },
    { .reg = 0x900b6, .val = 0x8 },
    { .reg = 0x900b7, .val = 0x370 },
    { .reg = 0x900b8, .val = 0x129 },
    { .reg = 0x900b9, .val = 0xa },
    { .reg = 0x900ba, .val = 0x3c8 },
    { .reg = 0x900bb, .val = 0x1a9 },
    { .reg = 0x900bc, .val = 0xc },
    { .reg = 0x900bd, .val = 0x408 },
    { .reg = 0x900be, .val = 0x199 },
    { .reg = 0x900bf, .val = 0x14 },
    { .reg = 0x900c0, .val = 0x790 },
    { .reg = 0x900c1, .val = 0x11a },
    { .reg = 0x900c2, .val = 0x8 },
    { .reg = 0x900c3, .val = 0x4 },
    { .reg = 0x900c4, .val = 0x18 },
    { .reg = 0x900c5, .val = 0xe },
    { .reg = 0x900c6, .val = 0x408 },
    { .reg = 0x900c7, .val = 0x199 },
    { .reg = 0x900c8, .val = 0x8 },
    { .reg = 0x900c9, .val = 0x8568 },
    { .reg = 0x900ca, .val = 0x108 },
    { .reg = 0x900cb, .val = 0x18 },
    { .reg = 0x900cc, .val = 0x790 },
    { .reg = 0x900cd, .val = 0x16a },
    { .reg = 0x900ce, .val = 0x8 },
    { .reg = 0x900cf, .val = 0x1d8 },
    { .reg = 0x900d0, .val = 0x169 },
    { .reg = 0x900d1, .val = 0x10 },
    { .reg = 0x900d2, .val = 0x8558 },
    { .reg = 0x900d3, .val = 0x168 },
    { .reg = 0x900d4, .val = 0x70 },
    { .reg = 0x900d5, .val = 0x788 },
    { .reg = 0x900d6, .val = 0x16a },
    { .reg = 0x900d7, .val = 0x1ff8 },
    { .reg = 0x900d8, .val = 0x85a8 },
    { .reg = 0x900d9, .val = 0x1e8 },
    { .reg = 0x900da, .val = 0x50 },
    { .reg = 0x900db, .val = 0x798 },
    { .reg = 0x900dc, .val = 0x16a },
    { .reg = 0x900dd, .val = 0x60 },
    { .reg = 0x900de, .val = 0x7a0 },
    { .reg = 0x900df, .val = 0x16a },
    { .reg = 0x900e0, .val = 0x8 },
    { .reg = 0x900e1, .val = 0x8310 },
    { .reg = 0x900e2, .val = 0x168 },
    { .reg = 0x900e3, .val = 0x8 },
    { .reg = 0x900e4, .val = 0xa310 },
    { .reg = 0x900e5, .val = 0x168 },
    { .reg = 0x900e6, .val = 0xa },
    { .reg = 0x900e7, .val = 0x408 },
    { .reg = 0x900e8, .val = 0x169 },
    { .reg = 0x900e9, .val = 0x6e },
    { .reg = 0x900ea, .val = 0x0 },
    { .reg = 0x900eb, .val = 0x68 },
    { .reg = 0x900ec, .val = 0x0 },
    { .reg = 0x900ed, .val = 0x408 },
    { .reg = 0x900ee, .val = 0x169 },
    { .reg = 0x900ef, .val = 0x0 },
    { .reg = 0x900f0, .val = 0x8310 },
    { .reg = 0x900f1, .val = 0x168 },
    { .reg = 0x900f2, .val = 0x0 },
    { .reg = 0x900f3, .val = 0xa310 },
    { .reg = 0x900f4, .val = 0x168 },
    { .reg = 0x900f5, .val = 0x1ff8 },
    { .reg = 0x900f6, .val = 0x85a8 },
    { .reg = 0x900f7, .val = 0x1e8 },
    { .reg = 0x900f8, .val = 0x68 },
    { .reg = 0x900f9, .val = 0x798 },
    { .reg = 0x900fa, .val = 0x16a },
    { .reg = 0x900fb, .val = 0x78 },
    { .reg = 0x900fc, .val = 0x7a0 },
    { .reg = 0x900fd, .val = 0x16a },
    { .reg = 0x900fe, .val = 0x68 },
    { .reg = 0x900ff, .val = 0x790 },
    { .reg = 0x90100, .val = 0x16a },
    { .reg = 0x90101, .val = 0x8 },
    { .reg = 0x90102, .val = 0x8b10 },
    { .reg = 0x90103, .val = 0x168 },
    { .reg = 0x90104, .val = 0x8 },
    { .reg = 0x90105, .val = 0xab10 },
    { .reg = 0x90106, .val = 0x168 },
    { .reg = 0x90107, .val = 0xa },
    { .reg = 0x90108, .val = 0x408 },
    { .reg = 0x90109, .val = 0x169 },
    { .reg = 0x9010a, .val = 0x58 },
    { .reg = 0x9010b, .val = 0x0 },
    { .reg = 0x9010c, .val = 0x68 },
    { .reg = 0x9010d, .val = 0x0 },
    { .reg = 0x9010e, .val = 0x408 },
    { .reg = 0x9010f, .val = 0x169 },
    { .reg = 0x90110, .val = 0x0 },
    { .reg = 0x90111, .val = 0x8b10 },
    { .reg = 0x90112, .val = 0x168 },
    { .reg = 0x90113, .val = 0x1 },
    { .reg = 0x90114, .val = 0xab10 },
    { .reg = 0x90115, .val = 0x168 },
    { .reg = 0x90116, .val = 0x0 },
    { .reg = 0x90117, .val = 0x1d8 },
    { .reg = 0x90118, .val = 0x169 },
    { .reg = 0x90119, .val = 0x80 },
    { .reg = 0x9011a, .val = 0x790 },
    { .reg = 0x9011b, .val = 0x16a },
    { .reg = 0x9011c, .val = 0x18 },
    { .reg = 0x9011d, .val = 0x7aa },
    { .reg = 0x9011e, .val = 0x6a },
    { .reg = 0x9011f, .val = 0xa },
    { .reg = 0x90120, .val = 0x0 },
    { .reg = 0x90121, .val = 0x1e9 },
    { .reg = 0x90122, .val = 0x8 },
    { .reg = 0x90123, .val = 0x8080 },
    { .reg = 0x90124, .val = 0x108 },
    { .reg = 0x90125, .val = 0xf },
    { .reg = 0x90126, .val = 0x408 },
    { .reg = 0x90127, .val = 0x169 },
    { .reg = 0x90128, .val = 0xc },
    { .reg = 0x90129, .val = 0x0 },
    { .reg = 0x9012a, .val = 0x68 },
    { .reg = 0x9012b, .val = 0x9 },
    { .reg = 0x9012c, .val = 0x0 },
    { .reg = 0x9012d, .val = 0x1a9 },
    { .reg = 0x9012e, .val = 0x0 },
    { .reg = 0x9012f, .val = 0x408 },
    { .reg = 0x90130, .val = 0x169 },
    { .reg = 0x90131, .val = 0x0 },
    { .reg = 0x90132, .val = 0x8080 },
    { .reg = 0x90133, .val = 0x108 },
    { .reg = 0x90134, .val = 0x8 },
    { .reg = 0x90135, .val = 0x7aa },
    { .reg = 0x90136, .val = 0x6a },
    { .reg = 0x90137, .val = 0x0 },
    { .reg = 0x90138, .val = 0x8568 },
    { .reg = 0x90139, .val = 0x108 },
    { .reg = 0x9013a, .val = 0xb7 },
    { .reg = 0x9013b, .val = 0x790 },
    { .reg = 0x9013c, .val = 0x16a },
    { .reg = 0x9013d, .val = 0x1f },
    { .reg = 0x9013e, .val = 0x0 },
    { .reg = 0x9013f, .val = 0x68 },
    { .reg = 0x90140, .val = 0x8 },
    { .reg = 0x90141, .val = 0x8558 },
    { .reg = 0x90142, .val = 0x168 },
    { .reg = 0x90143, .val = 0xf },
    { .reg = 0x90144, .val = 0x408 },
    { .reg = 0x90145, .val = 0x169 },
    { .reg = 0x90146, .val = 0xd },
    { .reg = 0x90147, .val = 0x0 },
    { .reg = 0x90148, .val = 0x68 },
    { .reg = 0x90149, .val = 0x0 },
    { .reg = 0x9014a, .val = 0x408 },
    { .reg = 0x9014b, .val = 0x169 },
    { .reg = 0x9014c, .val = 0x0 },
    { .reg = 0x9014d, .val = 0x8558 },
    { .reg = 0x9014e, .val = 0x168 },
    { .reg = 0x9014f, .val = 0x8 },
    { .reg = 0x90150, .val = 0x3c8 },
    { .reg = 0x90151, .val = 0x1a9 },
    { .reg = 0x90152, .val = 0x3 },
    { .reg = 0x90153, .val = 0x370 },
    { .reg = 0x90154, .val = 0x129 },
    { .reg = 0x90155, .val = 0x20 },
    { .reg = 0x90156, .val = 0x2aa },
    { .reg = 0x90157, .val = 0x9 },
    { .reg = 0x90158, .val = 0x8 },
    { .reg = 0x90159, .val = 0xe8 },
    { .reg = 0x9015a, .val = 0x109 },
    { .reg = 0x9015b, .val = 0x0 },
    { .reg = 0x9015c, .val = 0x8140 },
    { .reg = 0x9015d, .val = 0x10c },
    { .reg = 0x9015e, .val = 0x10 },
    { .reg = 0x9015f, .val = 0x8138 },
    { .reg = 0x90160, .val = 0x104 },
    { .reg = 0x90161, .val = 0x8 },
    { .reg = 0x90162, .val = 0x448 },
    { .reg = 0x90163, .val = 0x109 },
    { .reg = 0x90164, .val = 0xf },
    { .reg = 0x90165, .val = 0x7c0 },
    { .reg = 0x90166, .val = 0x109 },
    { .reg = 0x90167, .val = 0x0 },
    { .reg = 0x90168, .val = 0xe8 },
    { .reg = 0x90169, .val = 0x109 },
    { .reg = 0x9016a, .val = 0x47 },
    { .reg = 0x9016b, .val = 0x630 },
    { .reg = 0x9016c, .val = 0x109 },
    { .reg = 0x9016d, .val = 0x8 },
    { .reg = 0x9016e, .val = 0x618 },
    { .reg = 0x9016f, .val = 0x109 },
    { .reg = 0x90170, .val = 0x8 },
    { .reg = 0x90171, .val = 0xe0 },
    { .reg = 0x90172, .val = 0x109 },
    { .reg = 0x90173, .val = 0x0 },
    { .reg = 0x90174, .val = 0x7c8 },
    { .reg = 0x90175, .val = 0x109 },
    { .reg = 0x90176, .val = 0x8 },
    { .reg = 0x90177, .val = 0x8140 },
    { .reg = 0x90178, .val = 0x10c },
    { .reg = 0x90179, .val = 0x0 },
    { .reg = 0x9017a, .val = 0x478 },
    { .reg = 0x9017b, .val = 0x109 },
    { .reg = 0x9017c, .val = 0x0 },
    { .reg = 0x9017d, .val = 0x1 },
    { .reg = 0x9017e, .val = 0x8 },
    { .reg = 0x9017f, .val = 0x8 },
    { .reg = 0x90180, .val = 0x4 },
    { .reg = 0x90181, .val = 0x0 },
    { .reg = 0x90006, .val = 0x8 },
    { .reg = 0x90007, .val = 0x7c8 },
    { .reg = 0x90008, .val = 0x109 },
    { .reg = 0x90009, .val = 0x0 },
    { .reg = 0x9000a, .val = 0x400 },
    { .reg = 0x9000b, .val = 0x106 },
    { .reg = 0xd00e7, .val = 0x400 },
    { .reg = 0x90017, .val = 0x0 },
    { .reg = 0x9001f, .val = 0x29 },
    { .reg = 0x90026, .val = 0x68 },
    { .reg = 0x400d0, .val = 0x0 },
    { .reg = 0x400d1, .val = 0x101 },
    { .reg = 0x400d2, .val = 0x105 },
    { .reg = 0x400d3, .val = 0x107 },
    { .reg = 0x400d4, .val = 0x10f },
    { .reg = 0x400d5, .val = 0x202 },
    { .reg = 0x400d6, .val = 0x20a },
    { .reg = 0x400d7, .val = 0x20b },
    { .reg = 0x2003a, .val = 0x2 },
    { .reg = 0x200be, .val = 0x3 },
    { .reg = 0x2000b, .val = 0x7d },
    { .reg = 0x2000c, .val = 0xfa },
    { .reg = 0x2000d, .val = 0x9c4 },
    { .reg = 0x2000e, .val = 0x2c },
    { .reg = 0x12000b, .val = 0xc },
    { .reg = 0x12000c, .val = 0x19 },
    { .reg = 0x12000d, .val = 0xfa },
    { .reg = 0x12000e, .val = 0x10 },
    { .reg = 0x22000b, .val = 0x3 },
    { .reg = 0x22000c, .val = 0x6 },
    { .reg = 0x22000d, .val = 0x3e },
    { .reg = 0x22000e, .val = 0x10 },
    { .reg = 0x9000c, .val = 0x0 },
    { .reg = 0x9000d, .val = 0x173 },
    { .reg = 0x9000e, .val = 0x60 },
    { .reg = 0x9000f, .val = 0x6110 },
    { .reg = 0x90010, .val = 0x2152 },
    { .reg = 0x90011, .val = 0xdfbd },
    { .reg = 0x90012, .val = 0x2060 },
    { .reg = 0x90013, .val = 0x6152 },
    { .reg = 0x20010, .val = 0x5a },
    { .reg = 0x20011, .val = 0x3 },
    { .reg = 0x40080, .val = 0xe0 },
    { .reg = 0x40081, .val = 0x12 },
    { .reg = 0x40082, .val = 0xe0 },
    { .reg = 0x40083, .val = 0x12 },
    { .reg = 0x40084, .val = 0xe0 },
    { .reg = 0x40085, .val = 0x12 },
    { .reg = 0x140080, .val = 0xe0 },
    { .reg = 0x140081, .val = 0x12 },
    { .reg = 0x140082, .val = 0xe0 },
    { .reg = 0x140083, .val = 0x12 },
    { .reg = 0x140084, .val = 0xe0 },
    { .reg = 0x140085, .val = 0x12 },
    { .reg = 0x240080, .val = 0xe0 },
    { .reg = 0x240081, .val = 0x12 },
    { .reg = 0x240082, .val = 0xe0 },
    { .reg = 0x240083, .val = 0x12 },
    { .reg = 0x240084, .val = 0xe0 },
    { .reg = 0x240085, .val = 0x12 },
    { .reg = 0x400fd, .val = 0xf },
    { .reg = 0x10011, .val = 0x1 },
    { .reg = 0x10012, .val = 0x1 },
    { .reg = 0x10013, .val = 0x180 },
    { .reg = 0x10018, .val = 0x1 },
    { .reg = 0x10002, .val = 0x6209 },
    { .reg = 0x100b2, .val = 0x1 },
    { .reg = 0x101b4, .val = 0x1 },
    { .reg = 0x102b4, .val = 0x1 },
    { .reg = 0x103b4, .val = 0x1 },
    { .reg = 0x104b4, .val = 0x1 },
    { .reg = 0x105b4, .val = 0x1 },
    { .reg = 0x106b4, .val = 0x1 },
    { .reg = 0x107b4, .val = 0x1 },
    { .reg = 0x108b4, .val = 0x1 },
    { .reg = 0x11011, .val = 0x1 },
    { .reg = 0x11012, .val = 0x1 },
    { .reg = 0x11013, .val = 0x180 },
    { .reg = 0x11018, .val = 0x1 },
    { .reg = 0x11002, .val = 0x6209 },
    { .reg = 0x110b2, .val = 0x1 },
    { .reg = 0x111b4, .val = 0x1 },
    { .reg = 0x112b4, .val = 0x1 },
    { .reg = 0x113b4, .val = 0x1 },
    { .reg = 0x114b4, .val = 0x1 },
    { .reg = 0x115b4, .val = 0x1 },
    { .reg = 0x116b4, .val = 0x1 },
    { .reg = 0x117b4, .val = 0x1 },
    { .reg = 0x118b4, .val = 0x1 },
    { .reg = 0x12011, .val = 0x1 },
    { .reg = 0x12012, .val = 0x1 },
    { .reg = 0x12013, .val = 0x180 },
    { .reg = 0x12018, .val = 0x1 },
    { .reg = 0x12002, .val = 0x6209 },
    { .reg = 0x120b2, .val = 0x1 },
    { .reg = 0x121b4, .val = 0x1 },
    { .reg = 0x122b4, .val = 0x1 },
    { .reg = 0x123b4, .val = 0x1 },
    { .reg = 0x124b4, .val = 0x1 },
    { .reg = 0x125b4, .val = 0x1 },
    { .reg = 0x126b4, .val = 0x1 },
    { .reg = 0x127b4, .val = 0x1 },
    { .reg = 0x128b4, .val = 0x1 },
    { .reg = 0x13011, .val = 0x1 },
    { .reg = 0x13012, .val = 0x1 },
    { .reg = 0x13013, .val = 0x180 },
    { .reg = 0x13018, .val = 0x1 },
    { .reg = 0x13002, .val = 0x6209 },
    { .reg = 0x130b2, .val = 0x1 },
    { .reg = 0x131b4, .val = 0x1 },
    { .reg = 0x132b4, .val = 0x1 },
    { .reg = 0x133b4, .val = 0x1 },
    { .reg = 0x134b4, .val = 0x1 },
    { .reg = 0x135b4, .val = 0x1 },
    { .reg = 0x136b4, .val = 0x1 },
    { .reg = 0x137b4, .val = 0x1 },
    { .reg = 0x138b4, .val = 0x1 },
    { .reg = 0x20089, .val = 0x1 },
    { .reg = 0x20088, .val = 0x19 },
    { .reg = 0xc0080, .val = 0x2 },
    { .reg = 0xd0000, .val = 0x1 }
};

static struct dram_fsp_msg lpddr4_dram_fsp_msg[] = {
    {
        /* P0 4000mts 1D */
        .drate = 4000,
        .fw_type = IMX_FW_1D_IMAGE,
        .fsp_cfg = lpddr4_fsp0_cfg,
        .fsp_cfg_num = ARRAY_SIZE(lpddr4_fsp0_cfg),
    },
    {
        /* P1 400mts 1D */
        .drate = 400,
        .fw_type = IMX_FW_1D_IMAGE,
        .fsp_cfg = lpddr4_fsp1_cfg,
        .fsp_cfg_num = ARRAY_SIZE(lpddr4_fsp1_cfg),
    },
    {
        /* P1 100mts 1D */
        .drate = 100,
        .fw_type = IMX_FW_1D_IMAGE,
        .fsp_cfg = lpddr4_fsp2_cfg,
        .fsp_cfg_num = ARRAY_SIZE(lpddr4_fsp2_cfg),
    },
    {
        /* P0 4000mts 2D */
        .drate = 4000,
        .fw_type = IMX_FW_2D_IMAGE,
        .fsp_cfg = lpddr4_fsp0_2d_cfg,
        .fsp_cfg_num = ARRAY_SIZE(lpddr4_fsp0_2d_cfg),
    },
};
#endif /* IMX_SPL_BOOT */

static inline void poll_pmu_message_ready(void)
{
    unsigned int reg;

    do {
        reg = in32(IMX_DDR_PHY_BASE + (4 * 0xD0004));
    } while ((reg & 0x01) != 0U);
}

static inline void ack_pmu_message_recieve(void)
{
    unsigned int reg;

    out32(IMX_DDR_PHY_BASE + (4 * 0xD0031), 0x00);
    do {
        reg = in32(IMX_DDR_PHY_BASE + (4 * 0xD0004));
    } while ((reg & 0x01) == 0U);
    out32(IMX_DDR_PHY_BASE + (4 * 0xD0031), 0x01);
}

static inline unsigned int get_mail(void)
{
    unsigned int reg;

    poll_pmu_message_ready();
    reg = in32(IMX_DDR_PHY_BASE + (4 * 0xD0032));
    ack_pmu_message_recieve();

    return reg;
}

#if defined(IMX_SPL_BOOT)
static void wait_ddrphy_training_complete(void)
{
    unsigned int mail;

    while (1) {
        mail = get_mail();
        if (mail == 0x07) {
#if IMX_DDR_DEBUG
            kprintf("Training PASS.\n");
#endif
            break;
        } else if (mail == 0xFF) {
#if IMX_DDR_DEBUG
            kprintf("Training FAILED!\n");
#endif
            break;
        }
    }
}

static void ddrphy_init_read_msg_block(const enum fw_type_t type)
{

}

void ddrphy_init_set_dfi_clk(const uint32_t rate)
{
    switch (rate) {
    case 4000:
        /* DRAM_PLL_CLK = 1000MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                        IMX_CCM_TARGET_ROOT_MUX_VALUE(0)));
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(4) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(5)));
        break;
    case 400:
        /* SYSTEM_PLL2 / 2 = 400MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_ALT), (IMX_CCM_TARGET_ROOT_ENABLE_MASK|
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(2)));
        /* SYSTEM_PLL1_DIV5 / 2 = 80MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(3) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(2)));
        /* DRAM_ALT_CLK_ROOT = 400MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                        IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
        break;
    case 100:
        /* SYSTEM_PLL1_DIV8 = 100MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_ALT), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(2) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(1)));
        /* SYSTEM_PLL1_DIV20 = 40MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(2) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(2)));
        /* DRAM_ALT_CLK_ROOT = 400MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                        IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
        break;
    default:
        return;
    }
}

extern uint64_t __ddrPmuTrain_begin;

/**
 * Load DDR train firmware. (We need PHY iMEM PHY is 32KB padded)
 *
 * @param type Firmware type.
 */
static void ddr_load_train_firmware(const enum fw_type_t type)
{
    uint32_t tmp32, idx;
    uint32_t error = 0;
    unsigned long pr_to32, pr_from32;
    const unsigned long fw_offset = type ? IMEM_2D_OFFSET : 0;
    const unsigned long imem_start = (unsigned long)&__ddrPmuTrain_begin + fw_offset;
    const unsigned long dmem_start = imem_start + IMEM_LEN;

    pr_from32 = imem_start;
    pr_to32 = DDR_TRAIN_CODE_BASE_ADDR + 4 * IMEM_OFFSET_ADDR;
    for(idx = 0; idx < IMEM_LEN; ) {
        tmp32 = in32(pr_from32);
        out32(pr_to32, tmp32 & 0x0000FFFF);
        pr_to32 += 4;
        out32(pr_to32, (tmp32 >> 16) & 0x0000FFFF);
        pr_to32 += 4;
        pr_from32 += 4;
        idx += 4;
    }

    pr_from32 = dmem_start;
    pr_to32 = DDR_TRAIN_CODE_BASE_ADDR + (4 * DMEM_OFFSET_ADDR);
    for(idx = 0; idx < DMEM_LEN;){
        tmp32 = in32(pr_from32);
        out32(pr_to32, tmp32 & 0x0000FFFF);
        pr_to32 += 4;
        out32(pr_to32, (tmp32 >> 16) & 0x0000FFFF);
        pr_to32 += 4;
        pr_from32 += 4;
        idx += 4;
    }

#if IMX_DDR_DEBUG
    kprintf("check ddr4_pmu_train_imem code\n");
#endif
    pr_from32 = imem_start;
    pr_to32 = DDR_TRAIN_CODE_BASE_ADDR + (4 * IMEM_OFFSET_ADDR);
    for(idx = 0; idx < IMEM_LEN;){
        tmp32 = (in32(pr_to32) & 0x0000FFFF);
        pr_to32 += 4;
        tmp32 += ((in32(pr_to32) & 0x0000FFFF) << 16);

        if(tmp32 != in32(pr_from32)){
            error++;
        }
        pr_from32 += 4;
        pr_to32 += 4;
        idx += 4;
    }
#if IMX_DDR_DEBUG
    if(error){
        kprintf("check ddr4_pmu_train_imem code fail=%d\n", error);
    }else{
        kprintf("check ddr4_pmu_train_imem code pass\n");
    }

    kprintf("check ddr4_pmu_train_dmem code\n");
#endif
    pr_from32 = dmem_start;
    pr_to32 = DDR_TRAIN_CODE_BASE_ADDR + 4 * DMEM_OFFSET_ADDR;
    for(idx = 0x0; idx < DMEM_LEN;){
        tmp32 = (in32(pr_to32) & 0x0000ffff);
        pr_to32 += 4;
        tmp32 += ((in32(pr_to32) & 0x0000ffff) << 16);
        if(tmp32 != in32(pr_from32)){
#if IMX_DDR_DEBUG
            kprintf("0x%lx, 0x%lx\n", pr_from32, pr_to32);
#endif
            error++;
        }
        pr_from32 += 4;
        pr_to32 += 4;
        idx += 4;
    }

#if IMX_DDR_DEBUG
    if(error){
        kprintf("check ddr4_pmu_train_dmem code fail = %d\n", error);
    }else{
        kprintf("check ddr4_pmu_train_dmem code pass\n");
    }
#endif
}

void lpddr4_cfg_phy(void)
{
    int idx, j;
    struct dram_fsp_msg *fsp_msg;
    struct dram_cfg_param *dram_cfg;

    /* Initialize PHY configuration */
    dram_cfg = lpddr4_ddrphy_cfg;
    for (idx = 0; idx < ARRAY_SIZE(lpddr4_ddrphy_cfg); idx++) {
        /* Configure PHy registers */
        out32(IMX_DDR_PHY_BASE + (4 * dram_cfg->reg), dram_cfg->val);
        dram_cfg++;
    }

    /* Load the frequency setpoint message block config */
    fsp_msg = lpddr4_dram_fsp_msg;
    for (idx = 0; idx < ARRAY_SIZE(lpddr4_dram_fsp_msg); idx++) {
        /* Set dram PHY input clocks to desired frequency */
        ddrphy_init_set_dfi_clk(fsp_msg->drate);

        /* Load the dram training firmware image */
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0000), 0x00);
        ddr_load_train_firmware(fsp_msg->fw_type);

        /* Load the frequency set point message block parameter */
        dram_cfg = fsp_msg->fsp_cfg;
        for (j = 0; j < fsp_msg->fsp_cfg_num; j++) {
            out32(IMX_DDR_PHY_BASE + (4 * dram_cfg->reg), dram_cfg->val);
            dram_cfg++;
        }

        /**
         * Execute the firmware.
         * Running the firmware is a simply process to taking the
         * PMU out of reset and stall, then the firmware will be run
         * 1. reset the PMU;
         * 2. begin the execution;
         * 3. wait for the training done;
         * 4. read the message block result.
         */
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0000), 0x01);
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0099), 0x09);
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0099), 0x01);
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0099), 0x00);

        /* Wait for the training firmware to complete */
        wait_ddrphy_training_complete();

        /* Halt the microcontroller */
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0099), 0x01);

        /* Read the Message Block results */
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0000), 0x00);
        ddrphy_init_read_msg_block(fsp_msg->fw_type);
        out32(IMX_DDR_PHY_BASE + (4 * 0xD0000), 0x01);

        fsp_msg++;
    }

    /* Load PHY Init Engine Image */
    dram_cfg = lpddr4_phy_pie;
    for (idx = 0; idx < ARRAY_SIZE(lpddr4_phy_pie); idx++) {
        out32(IMX_DDR_PHY_BASE + (4 * dram_cfg->reg), dram_cfg->val);
        dram_cfg++;
    }
}
#endif /* IMX_SPL_BOOT */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_ddrphy_train.c $ $Rev: 980064 $")
#endif
