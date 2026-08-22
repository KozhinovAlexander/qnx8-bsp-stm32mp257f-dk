/*
 * Copyright 2022-2023 BlackBerry Limited.
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
 */


#ifndef BOARD_H_
#define BOARD_H_

/**
 * i.MX startup source file.
 *
 * @file       imx8mp/board.h
 * @addtogroup ipl
 * @{
 */

/* Peripheral PAD specific section */
/* USDHC */
#define IMX8MP_PAD_SETTINGS_USDHC (IMX_PAD_CTL_PE_PULL_ENABLED | IMX_PAD_CTL_PUE_PULL_UP | IMX_PAD_CTL_HYS_SCHMITT | IMX_PAD_CTL_SRE_FAST | IMX_PAD_CTL_DSE_2X)

/*!
 * @name IPL console UART configuration.
 */
/*@{*/
/** UART console base address */
#define IMX_CONSOLE_UART_BASE           IMX_UART2_BASE
/** UART console baud rate */
#define IMX_CONSOLE_UART_BAUD_RATE      115200U
/*@}*/

/** 8MHz from CPclk */
#define	IMX_COUNTER_FREQUENCY           8000000

/*@{*/
/** eSDC module base address */
#define IMX_eSDC_IPL_SEEK                64
#define IMX_eSDC_MODULE_BASE            IMX_USDHC2_BASE
#define IMX_eSDC_INPUT_FREQ             400000000
/*@}*/

/*@{*/
/** eMMC module base address */
#define IMX_eMMC_IPL_SEEK               IMX_eSDC_IPL_SEEK
#define IMX_eMMC_BOOT_IPL_SEEK          0
#define IMX_eMMC_MODULE_BASE            (IMX_USDHC3_BASE)
#define IMX_eMMC_INPUT_FREQ             400000000
#define IMX_eMMC_FUSE_FASTBT_ADDR       (IMX_HW_OCOTP_BOOT_CFG2)
#define IMX_eMMC_FUSE_FASTBT_POS        6
#define IMX_eMMC_FUSE_FASTBT_MASK       (1 << 6)
/*@}*/

/**
 * QNX-IFS image position in NOR flash memory.
 */
#define IMX_FLASH_IMAGE_ADDR            0x200000

/**
 *  DTB image position in NOR flash memory.
 */
#define IMX_FLASH_FDT_ADDR              0x100000 /* DTB image is started at 1MB boundary */

/*!
 * @name Configuration loading of QNX-IFS image
 */
/*@{*/
/**
 * Load QNX image from:\n
 *   0  - Boot source defined by key from debug console.\n
 *  'D' - serial download, using the 'sendnto' utility.\n
 *  'M' - SDMMC download, IFS filename MUST be 'QNX-IFS'.\n
 *  'E' - eMMC download, IFS filename MUST be 'QNX-IFS'.\n
 *  'F' - NOR flash download, IFS filename be at 0x100000 offset.
 */
#define IMX_QNX_IMAGE_LOAD_FROM         0
#define IMX_QNX_IMAGE_SCAN_SIZE         0x1000
/*@}*/

/**
 * Setting IMX_QSPI_BOOT to 1 enables QNX-IFS image download
 * from NOR flash memory.
 */
#define IMX_QSPI_BOOT                   1

/**
 * Setting IMX_HAB_AUTHENTICATE_CONFIG to 1
 * enables HAB images authenticate process.
 */
/*@{*/
#ifndef IMX_HAB_AUTHENTICATE_CONFIG
    #define IMX_HAB_AUTHENTICATE_CONFIG 0
#endif
/**
 * Setting IMX_HAB_AUTHENTICATE_DEBUG to 1 enables
 * HAB debug messages.
 */
#define IMX_HAB_AUTHENTICATE_DEBUG      0
/*@}*/

/*!
 * @name ARM Trusted Firmware configuration
 */
/*@{*/

#if !defined(IMX_SPL_BOOT)
    #define IMX_ATF_LENGTH              0x010000
    #define IMX_ARM_TRUSTED_FW_BINARY   "bl31.bin"
#else
    #define IMX_ATF_LENGTH              0x000000
#endif
/*@}*/
/*!
 * @name Start of Cortex-M7 core configuration
 */
/*@{*/
/* Setting CM7_CORE_BOOT to 1 enables Cortex-M7 core start */
#define IMX_CM7_CORE_BOOT               0

#if (!defined(IMX_SPL_BOOT) && (IMX_CM7_CORE_BOOT == 1))
    /** Cortex-M7 core firmware name */
    #define IMX_CM7_CORE_FIRMWARE       "cm7_tcm.bin"
    /** Cortex-M7 load address from CA53 perspective (TCML) */
    #define IMX_CM7_CORE_FW_LOAD_ADDR   0x007E0000
    /** Max. size of Cortex-M7 firmware */
    #define IMX_CM7_MAX_FW_LENGTH       0x00020000
#else
    /** Max. size of Cortex-M7 firmware */
    #define IMX_CM7_MAX_FW_LENGTH       0x00000000
#endif
/** Cortex-M7 vector table offset */
#define IMX_CM7_VECTOR_TABLE_OFFSET     0x00000000
/*@}*/
/**
 * Setting IMX_CACHE_EN to 1 enables I/D caches feature
 * (improve QNX-IFS scan or authentication time).
 */
#define IMX_CACHE_EN                    1

/** IPL memory base address */
#define IMX_FULL_IPL_MEM_BASE           0x40200000

#if defined(IMX_SPL_BOOT)
    /** IPL Device Tree Blob size */
    #define IMX_IPL_DTB_SIZE            0x00000000
    /** IPL memory base address */
    #define IMX_IPL_MEM_BASE            0x00920000
    /** IPL code memory size */
    #define IMX_IPL_MEM_SIZE            0xC000
    /** IPL stack size */
    #define IMX_IPL_STACK_SIZE          0x5000
#else
    /** IPL Device Tree Blob size */
    #define IMX_IPL_DTB_SIZE            0x00003000
    /** IPL memory base address */
    #define IMX_IPL_MEM_BASE            (IMX_FULL_IPL_MEM_BASE)
    /** IPL code memory size */
    #define IMX_IPL_MEM_SIZE            0x50000
    /** IPL stack size */
    #define IMX_IPL_STACK_SIZE          0x10000
#endif

/** IPL image offset - parameter of mkimage utility */
#define IMX_IPL_IMAGE_OFFSET            0x60000

/** SDRAM base address (used for MMU configuration) */
#define IMX_SDRAM_BASE                  0x40000000
/** SDRAM size (used for MMU configuration) */
#define IMX_SDRAM_SIZE                  (6144 * 1024 * 1024)

/**
 *  The address into which you load the IFS image MUST NOT
 *  overlap the area it will be run from.  The image will
 *  be placed into the correct location by 'image_setup'.
 *
 *  The image is configured to be uncompressed to, and
 *  run from, 0x40800000.  The compressed image is loaded
 *  higher up, and we must leave a big enough gap so they
 *  do not overlap when being decompressed.
 *  Loading the image to 0x48000000 leaves a 120MB gap.
 */
#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
  #define IMX_QNX_LOAD_ADDR               (0x48000000U - 0x40U) /* QNX-IFS load address - IVT size */
#else
  #define IMX_QNX_LOAD_ADDR               (0x48000000U)
#endif

/**
 * Setting IMX_DDR_DEBUG to 1 enables DDR init. debug messages.
 */
#define IMX_DDR_DEBUG                   0

/**
 * Setting IMX_PMIC_OVERDRIVE_ENABLED to 1 increases VDD_ARM and VDD_SOC to allow overdrive mode
 */
#define IMX_PMIC_OVERDRIVE_ENABLED      1

/** @} */ /* End of ipl */

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/board.h $ $Rev: 987589 $")
#endif
