/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2018-2019, 2022 NXP
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

#ifndef IMX_IPL_H_
#define IMX_IPL_H_

#include <soc/nxp/imx8/mp/mx8mp.h>

/**
 * i.MX IPL source file.
 *
 * @file       imx_ipl.h
 * @addtogroup ipl
 * @{
 */

#define IMX_LE_2_BE_32(l) \
    ((((l) & 0x000000FF) << 24) | \
    (((l) & 0x0000FF00) << 8)  | \
    (((l) & 0x00FF0000) >> 8)  | \
    (((l) & 0xFF000000) >> 24))

#define IMX_ALIGN(val, align_size) ((val + (align_size - 1)) & ~(align_size - 1))

#if defined(IMX_ASM_MACROS_EN)
    /* Branch code according the exception level */
    .macro  branch_el_mode, reg, label4el3, label4el2, label4el1
    mrs     \reg, CurrentEL
    cmp     \reg, 0x0C
    b.eq    \label4el3
    cmp     \reg, 0x08
    b.eq    \label4el2
    cmp     \reg, 0x04
    b.eq    \label4el1
    .endm
#else

typedef struct {
    uint32_t image_number;          /* Number of images 32 bits Number of images in container */
    uint32_t images_size;           /* Size of whole images */
} __attribute__((packed)) imx_boot_data_hdr_t;

typedef struct {
    uint64_t image_src_ptr;         /* Image source 64 bits Address of image in boot media */
    uint64_t image_dest_ptr;        /* Image destination 64 bits Absolute address of image in system memory */
    uint64_t image_entry_ptr;       /* Image0 entry 64 bits Entry point for image in system memory */
    uint32_t image_size;
    uint32_t reserved;
} __attribute__((packed)) imx_boot_data_image_t;

typedef struct {
    imx_boot_data_hdr_t data_hdr;   /* Boot data header */
    imx_boot_data_image_t image[5]; /* Boot data images */
} __attribute__((packed)) imx_boot_data_t;

typedef struct {
    uint8_t reserved0;
    uint8_t boot_dev_instance;      /* Boot device instance: The instance index of the boot device, starting from 0 */
    uint8_t boot_dev_type;          /* Boot device type: 0x1 - SD/eSD, 0x2 - MMC/eMMC, 0x3 - NAND */
    uint8_t reserved1;
    uint32_t arm_core_freq;
    uint32_t axi_bus_freq;
    uint32_t ddr_freq;
    uint32_t gpt1_input_freq;
    uint32_t reserved2[3];
} __attribute__((packed)) imx_boot_info_t;

#define IMX_BOOT_DEV_SD_CARD        0x01
#define IMX_BOOT_DEV_eMMC_CHIP      0x02
#define IMX_BOOT_DEV_NAND_CHIP      0x03
#define IMX_BOOT_DEV_QSPI_CHIP      0x04

#define IMX_IVT_SIZE                0x20
#define IMX_CSF_PAD_SIZE            0x2000
#define IMX_ALIGN_SIZE              0x1000
#define IMX_CACHE_LINE_SIZE         64

#define IMX_IVT_HEADER_MAGIC        0xD1
#define IMX_IVT_TOTAL_LENGTH        0x20
#define IMX_HAB_MAJ_VER             0x40
#define IMX_HAB_MAJ_MASK            0xF0

#endif

#ifndef __ASM__
#if defined(IMX_SPL_BOOT)
void imx_init_system_counter(void);
void imx_init_clocks(void);
void imx_init_pinmux(void);
void imx_init_ddr(void);
void lpddr4_cfg_phy(void);
void ddrphy_init_set_dfi_clk(uint32_t rate);
#if IMX_PMIC_OVERDRIVE_ENABLED
void imx_init_pmic(void);
#endif
#else
#if (IMX_CM7_CORE_BOOT == 1)
void imx_start_cortex_m7_core(void);
#endif
#endif

void imx_init_console(void);
#if (!defined(IMX_SPL_BOOT) && (IMX_CACHE_EN == 1))
void imx_enable_mmu(void);
void imx_disable_mmu(void);
#endif

void imx_init_uart(unsigned port, unsigned baud, unsigned clk);
void delay(unsigned dly);
void _start(void);

#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
int imx_hab_authenticate_image(paddr_t img_addr, uint32_t img_size, uint32_t ivt_offset);
void imx_get_hab_status(void);
#endif

#endif /* __ASM__ */
#endif /* IMX_IPL_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_ipl.h $ $Rev: 980064 $")
#endif
