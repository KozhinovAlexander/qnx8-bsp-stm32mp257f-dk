/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017-2019, 2022 NXP
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

#include "sdma.h"
#include "microcode.h"

/**
 * @file        src/lib/dma/sdma/imx/script.c
 * @addtogroup  sdma
 * @{
 */

/* ROM Script locations */
#define AP_2_AP_ADDR        644
#define AP_2_MCU_ADDR       685
#define MCU_2_AP_ADDR       749
#define UART_2_MCU_ADDR     819
#define SHP_2_MCU_ADDR      893
#define MCU_2_SHP_ADDR      962
#define UARTSH_2_MCU_ADDR   1034
#define SPDIF_2_MCU_ADDR    1102
#define MCU_2_SPDIF_ADDR    1136

/* RAM Script locations */
#define I2C_2_MCU_ADDR          6144
#define MCU_2_ECSPI_ADDR        6178
#define MCU_2_I2C_ADDR          6269
#define MCU_2_SAI_ADDR          6308
#define MCU_2_SSISH_ADDR        6459
#define P_2_P_ADDR              6548
#define SAI_2_MCU_ADDR          6862
#define SSISH_2_MCU_ADDR        7013
#define UART_2_MCU_FIXED_ADDR   7097
#define UARTSH_2_MCU_FIXED_ADDR 7189
#define ZCANFD_2_MCU_ADDR       7263
#define ZHDMI_DMA_ADDR          7364
#define ZQSPI_2_MCU_ADDR        7419
#define ZZMCU_2_QSPI_ADDR       7521

///////////////////////////////////////////////////////////////////////////////
//                            PUBLIC FUNCTIONS                               //
///////////////////////////////////////////////////////////////////////////////

/**
 * Initializes sdma_scriptinfo_t structure.
 *
 * @param scriptinfo - Address of sdma_scriptinfo_t structure to be initialized.
 *
 * @return Always returns EOK.
 */
int sdmascript_lookup(sdma_scriptinfo_t * scriptinfo)
{

    /* Get ram microcode data... */
    scriptinfo->ram_microcode_info.p    =
        sdma_code;                    /* Start address of RAM image in process address space */
    scriptinfo->ram_microcode_info.addr = SDMA_RAM_CODE_START_ADDR; /* Start address of RAM image in SDMA RAM */
    scriptinfo->ram_microcode_info.size = sizeof(sdma_code);            /* Size of RAM image */

    /* Populate the scriptinfo struct with script addresses. */
    scriptinfo->script_addr_arr[SDMA_CHTYPE_AP_2_AP]            = AP_2_AP_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_MCU_2_AP]           = MCU_2_AP_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_AP_2_MCU]           = AP_2_MCU_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_MCU_2_SHP]          = MCU_2_SHP_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_SHP_2_MCU]          = SHP_2_MCU_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_UARTSH_2_MCU]       = UARTSH_2_MCU_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_MCU_2_SPDIF]        = MCU_2_SPDIF_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_SPDIF_2_MCU]        = SPDIF_2_MCU_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_UART_2_MCU]         = UART_2_MCU_ADDR;
    scriptinfo->script_addr_arr[SDMA_CHTYPE_ZQSPI_2_MCU]        = ZQSPI_2_MCU_ADDR;

    return EOK;
}

/** @} */ /* end of sdma */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/sdma/imx8mp3/script.c $ $Rev: 963555 $")
#endif
