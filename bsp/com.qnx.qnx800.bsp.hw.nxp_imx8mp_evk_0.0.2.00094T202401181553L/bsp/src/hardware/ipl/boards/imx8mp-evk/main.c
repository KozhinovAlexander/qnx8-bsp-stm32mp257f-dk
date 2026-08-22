/*
 * Copyright 2016, 2022-2023 BlackBerry Limited.
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
 */

#include "ipl.h"
#include <hw/inout.h>
#include "fat-fs.h"
#include <sdmmc/sdmmc.h>
#include <aarch64/imx8_sdhc.h>
#include <imx8_flexspi_ipl.h>
#include <sys/srcversion.h>
#include <libfdt.h>
#include <fdt.h>
#include <soc/nxp/imx8/common/imx_smc_call.h>
#include <soc/nxp/imx8/mp/imx_ccm_analog.h>
#include <soc/nxp/imx8/mp/imx_src.h>
#include <soc/nxp/imx8/mp/imx_ocotp.h>
#include "board.h"
#include "imx_ipl.h"

static int fdt_loaded = 1;


/**
 * i.MX IPL source file.
 *
 * @file       ipl/boards/imx8mp/main.c
 * @addtogroup ipl
 * @{
 */

/* The buffer used by fat-fs.c as the common buffer */
static unsigned char data_buf[FAT_COMMON_BUF_SIZE];

/**
 * Software delay loop.
 *
 * @param dly Delay loop count.
 */
void delay(unsigned dly)
{
    volatile int j;

    while (dly--) {
        for (j = 0; j < 32; j++);
    }
}

/**
 * Read processor fuse at given address.
 *
 * @param offset Address of the fuse register.
 * @param mask Mask of the fuse bits.
 *
 * @return Return fuse value.
 */
static unsigned imx_read_fuse(const unsigned offset, const unsigned mask)
{
    unsigned value;
    value = in32(IMX_OCOTP_CTRL_BASE + offset);
    value &= mask;

    return value;
}

#if IMX_QSPI_BOOT

static int fdt_image_scan(const void *const fdt)
{
    if (fdt_magic(fdt) == FDT_MAGIC) {
        /* Complete tree */
        if (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION) {
            return -FDT_ERR_BADVERSION;
        }
        if (fdt_last_comp_version(fdt) > FDT_LAST_SUPPORTED_VERSION) {
            return -FDT_ERR_BADVERSION;
        }
    } else if (fdt_magic(fdt) == (~FDT_MAGIC)) {
        /* Unfinished sequential-write blob */
        if (fdt_size_dt_struct(fdt) == 0) {
            return -FDT_ERR_BADSTATE;
        }
    } else {
        return -FDT_ERR_BADMAGIC;
    }

    return 0;
}
/**
 * Load QNX image file from flash memory to DDR memory.
 *
 * @param dest_addr     QNX image DDR memory address.
 * @param source_addr   QNX image flash memory address.
 *
 * @return              Execution status.
 */
static inline int flexspi_load_file(const unsigned long dest_addr, const unsigned source_addr)
{
    flexspi_conf_t flexspi_conf = {
            .rx_fifo_size = 512,        /* Rx FIFO size */
            .pads = 1,                  /* Pads number */
            .smpl = 0,                  /* Sample clock source */
            .lut0 = 0x0818040BU,        /* Lookup table 0 */
            .lut1 = 0x24043008U,        /* Lookup table 1 */
            .lut2 = 0U,                 /* Lookup table 2 */
            .lut3 = 0U,                 /* Lookup table 3 */
    };
    flexspi_t       flexspi;
    int          status = -1;
    struct       startup_header startup_hdr; /* Image header info structure */
    unsigned int imagesize;

    flexspi.conf = &flexspi_conf;
    imx8_flexspi_init(&flexspi, IMX_FLEXSPI0_BASE, 0);

    status = imx8_flexspi_read(&flexspi, source_addr, (unsigned long)&startup_hdr, sizeof(startup_hdr));
    if (status == FLEXSPI_OK) {
        imagesize = startup_hdr.stored_size;
        kprintf("QNX image detected, size in hex: %x\n", imagesize);
        status = imx8_flexspi_read(&flexspi, source_addr, dest_addr, imagesize);
        if (status == -1) {
            kprintf("QNX-IFS image load failed\n");
            return status;
        }
    }

    /* load DTB to memory */
    status = imx8_flexspi_read(&flexspi, IMX_FLASH_FDT_ADDR,
                              IMX_FDT_LOAD_ADDR, sizeof(struct fdt_header));
    if (status == FLEXSPI_OK) {
        const void *const fdt_blob = (void *) IMX_FDT_LOAD_ADDR;
	if ((fdt_image_scan(fdt_blob)) == 0) {
	    const unsigned fdt_size = fdt_totalsize(fdt_blob);
	    kprintf("FDT detected, size in hex: %x\n", fdt_size);
            status = imx8_flexspi_read(&flexspi, IMX_FLASH_FDT_ADDR,
                                      IMX_FDT_LOAD_ADDR, fdt_size);
	} else {
	       kprintf("FDT found at 0x%x is not valid\n", IMX_FDT_LOAD_ADDR);
	       fdt_loaded = 0;
	}
    }

    return status;
}

/**
 * Read images and jump to the entry point of the first image.
 *
 * @return Image load status:  0 - no error; -1 - error
 */
static inline int flexspi_load_images(void)
{
    flexspi_conf_t flexspi_conf = {
            .rx_fifo_size = 512,        /* Rx FIFO size */
            .pads = 1,                  /* Pads number */
            .smpl = 0,                  /* Sample */
            .lut0 = 0x0818040BU,
            .lut1 = 0x24043008U,
            .lut2 = 0U,
            .lut3 = 0U,
    };
    flexspi_t       flexspi;
    uint32_t        image_number;
    imx_boot_data_t *boot_data;
    uint32_t        idx;
    int status;

    flexspi.conf = &flexspi_conf;
    imx8_flexspi_init(&flexspi, IMX_FLEXSPI0_BASE, 0);
    /* Read images and jump to the entry point of the first image */

    status = imx8_flexspi_read(&flexspi, IMX_IPL_IMAGE_OFFSET, (unsigned long)data_buf, 512);
    if (status == FLEXSPI_OK) {
        if (*((uint32_t *)data_buf) == IMX_LE_2_BE_32(0xD00DFEED)) {
            boot_data = (imx_boot_data_t *)&data_buf[8];
            image_number = boot_data->data_hdr.image_number;
            for (idx = 0; idx < image_number; idx++) {
                status = imx8_flexspi_read(&flexspi, IMX_IPL_IMAGE_OFFSET + boot_data->image[idx].image_src_ptr,
                        (unsigned long)boot_data->image[idx].image_dest_ptr,
                        boot_data->image[idx].image_size);
                if (status != FLEXSPI_OK) {
                    kprintf("Read images failed.\n");
                    goto error;
                }
            }
            jump(boot_data->image[0].image_entry_ptr);
        } else {
            kprintf("IVT check failed!\n");
            goto error;
        }
    }
    return 0;
error:
    return -1;
}
#endif

/**
 *  Load QNX image file from SD, eMMC memory to DDR memory.
 *
 * @param card_type Memory device type (eSDC, eMMC).
 * @param address   QNX image load address.
 * @param file_name Image file name (e.g. QNX-IFS).
 *
 * @return          QNX image file load status:  SDMMC_OK - no error; SDMMC_ERROR - SDMMC error
 */
static inline int usdhc_load_file(const card_type_t card_type, const long address, const char * const file_name, const uint32_t cid)
{
    sdmmc_t sdmmc;
    sdmmc_ext_t ext = {0};
    int status;

    ext.chip_type = cid & 0x1F;
    ext.chip_rev = (cid >> 5) & 0x0F;
    ext.tuning_flags = IMX_USDHC_TUNING_MANUAL;

    if (card_type == eSDC) {
        /* Initialize the sdmmc interface and card, 400MHz clk */
        ext.dev_flags = HC_FLAG_DEV_SD;
        imx8_init_hc(&sdmmc, IMX_eSDC_MODULE_BASE, IMX_eSDC_INPUT_FREQ, SDMMC_VERBOSE_LVL_0, &ext);
        sdmmc.caps &= ~HC_CAP_SV_1_8V;
        status = sdmmc_init_sd(&sdmmc);
        if (status != SDMMC_OK) {
            kprintf("SD card init failed\n");
            return SDMMC_ERROR;
        }
    } else if (card_type == eMMC) {
        /* Initialize the emmc interface and card, 400MHz clk */
        ext.dev_flags = HC_FLAG_DEV_MMC;
        ext.tuning_flags = IMX_USDHC_TUNING_STANDARD;
        imx8_init_hc(&sdmmc, IMX_eMMC_MODULE_BASE, IMX_eMMC_INPUT_FREQ, SDMMC_VERBOSE_LVL_0, &ext);
        sdmmc.caps &= ~HC_CAP_SV_1_8V;
        status = sdmmc_init_mmc(&sdmmc);
        if (status != SDMMC_OK) {
            kprintf("eMMC flash init failed\n");
            return SDMMC_ERROR;
        }
    } else {
        kprintf("Unknown card type\n");
        return SDMMC_ERROR;
    }

    status = fat_copy_named_file((blkdev_t *)&sdmmc, (unsigned char *)address, (char *)file_name);
    if (status != SDMMC_OK) {
        kprintf("Failed to load image\n");
        return SDMMC_ERROR;
    }

    status = fat_copy_named_file((blkdev_t *)&sdmmc, (unsigned char *)IMX_FDT_LOAD_ADDR, (char *)IMX_FDT_FILE_NAME);
    if (status != SDMMC_OK) {
         kprintf("Failed to load fdt, please ensure dtb file is renamed to IMX-DTB\n");
	 fdt_loaded = 0;
    }

    return SDMMC_OK;
}

/**
 * Read images and jump to the entry point of the first image.
 *
 * @param card_type Memory device type (eSDC, eMMC).
 *
 * @return Image load status:  SDMMC_OK - no error; SDMMC_ERROR - SDMMC erron.cr
 */
static inline int usdhc_load_images(const card_type_t card_type, const uint32_t cid)
{
    sdmmc_t         sdmmc = {0};
    sdmmc_ext_t     ext = {0};
    int             status = SDMMC_OK;
    uint32_t        image_number;
    uint32_t        image_start_addr, total_image_size;
#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
    uint32_t        image_size, ivt_offset;
#endif
    imx_boot_data_t *boot_data;
    uint32_t        idx;
    unsigned        device_seek = 0;
    unsigned        emmc_boot_part = 0;

    ext.chip_type = cid & 0x1F;
    ext.chip_rev = (cid >> 5) & 0x0F;
    ext.tuning_flags = IMX_USDHC_TUNING_STANDARD;

    if (card_type == eSDC) {
        /* Initialize the sdmmc interface and card */
        device_seek = IMX_eSDC_IPL_SEEK;
        ext.dev_flags = HC_FLAG_DEV_SD;
        imx8_init_hc(&sdmmc, IMX_eSDC_MODULE_BASE, IMX_eSDC_INPUT_FREQ, SDMMC_VERBOSE_LVL_0, &ext);
        sdmmc.caps &= ~HC_CAP_SV_1_8V;
        status = sdmmc_init_sd(&sdmmc);
        if (status != SDMMC_OK) {
            kprintf("SD card init failed\n");
            status = SDMMC_ERROR;
            goto done;
        }
    } else if (card_type == eMMC) {
        device_seek = IMX_eMMC_IPL_SEEK;
        ext.dev_flags = HC_FLAG_DEV_MMC;
        imx8_init_hc(&sdmmc, IMX_eMMC_MODULE_BASE, IMX_eMMC_INPUT_FREQ, SDMMC_VERBOSE_LVL_0, &ext);
        sdmmc.caps &= ~HC_CAP_SV_1_8V;
        status = sdmmc_init_sd(&sdmmc);
        if (status != SDMMC_OK) {
            kprintf("eMMC flash init failed\n");
            status = SDMMC_ERROR;
            goto done;
        }
        if (imx_read_fuse(IMX_eMMC_FUSE_FASTBT_ADDR, IMX_eMMC_FUSE_FASTBT_MASK)) {
            kprintf("eMMC fast boot detected\n");
        }
        emmc_boot_part = sdmmc_get_mmc_boot_partition(&sdmmc);
        if (emmc_boot_part) {
            device_seek = IMX_eMMC_BOOT_IPL_SEEK;
            if (sdmmc_set_mmc_partition(&sdmmc, emmc_boot_part)) {
                status = SDMMC_ERROR;
                goto done;
            }
        }
    } else {
        kprintf("Unknown card type\n");
        status = SDMMC_ERROR;
        goto done;
    }
    /* Read images and jump to the entry point of the first image */
    if (sdmmc_read_write(&sdmmc, data_buf, 0x2C0 + device_seek, 1, 1) == SDMMC_OK) {
        if (*((uint32_t *)data_buf) == IMX_LE_2_BE_32(0xD00DFEED)) {
            boot_data = (imx_boot_data_t *)&data_buf[8];
            image_number = boot_data->data_hdr.image_number;
            total_image_size = boot_data->data_hdr.images_size;
            /* Calculate image start address, aligned on cache line address */
            image_start_addr = (IMX_FULL_IPL_MEM_BASE - (IMX_ALIGN_SIZE + IMX_CSF_PAD_SIZE) - 512 - (IMX_CACHE_LINE_SIZE - 1)) &
                          ~(IMX_CACHE_LINE_SIZE - 1);
            /* Load whole second image to DDR memory */
            if (sdmmc_read_write(&sdmmc, (void *)(unsigned long)image_start_addr, 0x2C0 + device_seek,
                    (total_image_size + 0x1FF) / 0x200, 1) != SDMMC_OK) {
                kprintf("Read images failed.\n");
                goto done;
            }
#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
            ivt_offset = ((IMX_LE_2_BE_32(((uint32_t *)data_buf)[1]) +
                         IMX_ALIGN_SIZE - 1) & ~(IMX_ALIGN_SIZE - 1));
            image_size = ivt_offset + IMX_IVT_SIZE + IMX_CSF_PAD_SIZE;

#if (IMX_HAB_AUTHENTICATE_DEBUG == 1)
            kprintf("Image HAB parameters:\n");
            kprintf("  Image start: 0x%x\n", image_start_addr);
            kprintf("  IVT offset:  0x%x\n", ivt_offset);
            kprintf("  Image size:  0x%x\n", image_size);
#endif
            if (imx_hab_authenticate_image(image_start_addr, image_size, ivt_offset) == 0U) {
#endif
                /* Copy images to the requested address */
                for (idx = 0; idx < image_number; idx++) {
                    copy(boot_data->image[idx].image_dest_ptr, image_start_addr + boot_data->image[idx].image_src_ptr, boot_data->image[idx].image_size);
                }
                /* Jump to first image destination address */
                jump(boot_data->image[0].image_entry_ptr);
#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
            } else {
                kprintf("IPL images authentication failed!\n");
                /* Print HAB status if images were not successfully authenticated */
                kprintf("HAB status:\n");
                imx_get_hab_status();
            }
#endif
        } else {
            kprintf("IVT check failed!\n");
            status = SDMMC_ERROR;
        }
    }

done:
    if (card_type == eMMC) {
        if (sdmmc_get_mmc_boot_partition(&sdmmc)) {
            sdmmc_set_mmc_partition(&sdmmc, 0);
        }
    }
    return status;
}

/**
 * Initial Program Loader main function. Initializes board (AIPS, clocks, pin routing, console,...),
 * loads QNX image to DDR RAM and start it.
 *
 * @return  Always 0 (never reach, jump to startup code).
 */
int main(void)
{
    paddr_t image;
    unsigned char boot_key = IMX_QNX_IMAGE_LOAD_FROM;
    int chip_rev = IMX_CHIP_REV_B;

#if !defined(IMX_SPL_BOOT)
    uint64_t atf_commit = 0;
#endif

#if defined(IMX_SPL_BOOT)
    imx_boot_info_t **imx_boot_info;
    int result;
#endif

    /* Initialize IPL serial console */
    imx_init_console();

#if defined(IMX_SPL_BOOT)
    /* Initialize PINs routing needed for IPL */
    imx_init_pinmux();

#if IMX_PMIC_OVERDRIVE_ENABLED
    /* Initialize PMIC voltage outputs */
    imx_init_pmic();
#endif

    /* Initialize the system clocks */
    imx_init_clocks();

    /* Initialize the system counter */
    imx_init_system_counter();

    /* Initialize DDR controller */
    imx_init_ddr();

    /* Override default load source according to boot pin configuration */
    imx_boot_info = (imx_boot_info_t **)IMX_BOOT_INFORMATION_BASE;
    switch ((*imx_boot_info)->boot_dev_type) {
    case IMX_BOOT_DEV_SD_CARD:                             /* SD card or eSD chip */
        kprintf("Boot device: SD card\n");
        /* Load/start the next images */
        result = usdhc_load_images(eSDC, chip_rev);
        break;
    case IMX_BOOT_DEV_eMMC_CHIP:                           /* MMC card or eMMC chip */
        kprintf("Boot device: eMMC chip\n");
        /* Load/start the next images */
        result = usdhc_load_images(eMMC, chip_rev);
        break;
#if IMX_QSPI_BOOT
    case IMX_BOOT_DEV_QSPI_CHIP:                           /* NOR or serial NAND chip */
        kprintf("Boot device: NOR/serial NAND chip\n");
        /* Load/start the next images */
        result = flexspi_load_images();
        break;
#endif
    default:
#if IMX_QSPI_BOOT
        if (((in32(IMX_SRC_BASE + IMX_SRC_SBMR1) >> 12) & 0x07) == IMX_BOOT_DEV_QSPI_CHIP) {
            result = flexspi_load_images();                /* FlexSPI detection failed! */
        } else
#endif
        {
            kprintf("Unsupported boot device type!\n");
            result = SDMMC_ERROR;
        }
        break;
    }

    if (result != SDMMC_OK) {
        kprintf("IPL/ATF load images failed at boot source: 0x%x\n", (*imx_boot_info)->boot_dev_type);
    }
    /* Never reach here */
    while (1)
    {
    }
#else
    #if (IMX_CM7_CORE_BOOT == 1)
    /* Start Cortex-M7 core */
    imx_start_cortex_m7_core();
    #endif

    kprintf("\nWelcome to QNX OS Initial Program Loader for NXP i.MX8MPlus Evaluation Kit Board (ARM Cortex-A53)\n");
    atf_commit = imx_sec_firmware_psci(IMX_FSL_SIP_BUILDINFO, IMX_FSL_SIP_BUILDINFO_GET_COMMITHASH, 0x00, 0x00, 0x00);
    if (atf_commit != (uint64_t)IMX_PSCI_NOT_SUPPORTED) {
        kprintf("ATF commit: %s\n", (char *)&atf_commit);
    }
#endif

    while (1) {
        image = IMX_QNX_LOAD_ADDR;
        if (boot_key == 0) {
            kprintf("Command:\n");
            kprintf("Press 'D' for serial download, using the 'sendnto' utility\n");
            kprintf("Press 'M' for SDMMC download, IFS filename MUST be 'QNX-IFS'.\n");
            kprintf("Press 'E' for eMMC download, IFS filename MUST be 'QNX-IFS'.\n");
#if IMX_QSPI_BOOT
            kprintf("Press 'F' for NOR flash download, image offsets MUST be - DTB:0x100000, IFS:0x200000.\n");
#endif
            boot_key = ser_getchar();
        }
        switch (boot_key) {
            case 'D':
            case 'd':
                kprintf("send image now...\n");
                if (image_download_ser(image)) {
                    kprintf("download failed...\n");
                    boot_key = 0;
                } else {
                    kprintf("download OK...\n");
                }
                break;
            case 'M':
            case 'm':
                kprintf("SDMMC download...\n");
		if (usdhc_load_file(eSDC, image, "QNX-IFS", chip_rev) == 0) {
                    kprintf("load image done.\n");
                    /* Proceed to image scan */
                } else {
                    kprintf("Load image failed.\n");
                    boot_key = 0;
                }
                break;
            case 'e':
            case 'E':
                kprintf("eMMC download...\n");
                if (usdhc_load_file(eMMC, image, "QNX-IFS", chip_rev) == 0) {
                    kprintf("load image done.\n");
                    /* Proceed to image scan */
                } else {
                    kprintf("Load image failed.\n");
                    boot_key = 0;
                }
                break;
#if IMX_QSPI_BOOT
            case 'F':
            case 'f':
                kprintf("NOR flash download...\n");
                if (flexspi_load_file(image, IMX_FLASH_IMAGE_ADDR) == 0) {
                    kprintf("Load image done.\n");
                    /* Proceed to image scan */
                } else {
                    kprintf("Load image failed.\n");
                    boot_key = 0;
                }
                break;
#endif
            default:
                kprintf("Unknown command.\n");
                boot_key = 0;
                break;
        }
        if (boot_key == 0) {
            continue;
        }
#if (!defined(IMX_SPL_BOOT) && (IMX_CACHE_EN == 1))
        /* Enable DCache and MMU */
        imx_enable_mmu();
#endif
#if (!defined(IMX_SPL_BOOT) && (IMX_HAB_AUTHENTICATE_CONFIG == 1))
        uint32_t        image_size, ivt_offset = 0x00;

        image_size = ((uint32_t *)image)[9];
#if (IMX_HAB_AUTHENTICATE_DEBUG == 1)
        kprintf("Image HAB parameters:\n");
        kprintf("  Image start:  0x%x\n", image);
        kprintf("  IVT offset:   0x%x\n", ivt_offset);
        kprintf("  Image size:   0x%x\n", image_size);
#endif
        if (imx_hab_authenticate_image(image, image_size, ivt_offset) != 0U) {
            kprintf("QNX-IFS image authentication failed!\n");
            /* Print HAB status if images were not successfully authenticated */
            imx_get_hab_status();
            BootKey = 0;
            continue;
        }
#endif
        /* Scan loaded image in DDR RAM memory */
        image = image_scan(image, image + IMX_QNX_IMAGE_SCAN_SIZE, 1);
#if (!defined(IMX_SPL_BOOT) && (IMX_CACHE_EN == 1))
        /* Disable DCache and MMU */
        imx_disable_mmu();
#endif
        if (image != (paddr_t)-1) {
            kprintf("Found image               @ 0x%x\n", image);
            image_setup(image);

            if (fdt_loaded == 1) {
                kprintf("Loading FDT               @ 0x%x\n", IMX_FDT_LOAD_ADDR);
            }
            kprintf("Jumping to startup        @ 0x%x\n\n", startup_hdr.startup_vaddr);

            image_start_with_fdt(image, IMX_FDT_LOAD_ADDR);

            /* Never reach here */
            return 0;
        }
        kprintf("Image_scan failed...\n");
    }
    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/main.c $ $Rev: 988107 $")
#endif
