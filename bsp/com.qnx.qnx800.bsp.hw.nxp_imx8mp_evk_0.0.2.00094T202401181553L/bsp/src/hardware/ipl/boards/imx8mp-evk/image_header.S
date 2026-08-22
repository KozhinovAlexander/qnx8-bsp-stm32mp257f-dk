/*
 * $QNXLicenseC:
 * Copyright 2016, 2022 BlackBerry Limited.
 * Copyright 2018-2019 NXP
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

#include "imx_ipl.h"
#include "board.h"

#if defined(IMX_SPL_BOOT)
    /* ************************************** */
    /* DDR PMU train FW section               */
    /* ************************************** */
    .section ".ddrPmuTrainFw", "a"
        .incbin "lpddr4_pmu_train.bin"
#else
    /* ************************************** */
    /* DDR PMU train FW section               */
    /* ************************************** */
    .section ".DeviceTreeSection", "a"

    dtb_base:
        ftd_magic:                .word IMX_LE_2_BE_32(0xD00DFEED) /* Header Magic Number */
        ftd_totalsize:            .word IMX_LE_2_BE_32(bd_images_end -  dtb_base) /* Total size of DT block */

    /* Boot data structure */
    bd_base:
        bd_hdr_image_number:      .word (0x00000002 + IMX_CM7_CORE_BOOT) /* Number of images 32 bits Number of images in container */
#if (IMX_CM7_CORE_BOOT == 1)
        bd_hdr_images_size:       .word (IMX_IPL_DTB_SIZE + IMX_ATF_LENGTH + __ipl_code_size + __cm7_code_size) /* Size of whole images */
#else
        bd_hdr_images_size:       .word (IMX_IPL_DTB_SIZE + IMX_ATF_LENGTH + __ipl_code_size) /* Size of whole images */
#endif
        /* ATF image */
        bd_image0_base:
            bd_image0_src_ptr:        .quad (_atf_fw_start - dtb_base) /* Image0 source 64 bits Address of image in boot media */
            bd_image0_dest_ptr:       .quad (0x00970000)           /* Image0 destination 64 bits Absolute address of image in system memory */
            bd_image0_entry_ptr:      .quad (0x00970000)           /* Image0 entry 64 bits Entry point for image in system memory */
            bd_image0_size:           .word (IMX_ATF_LENGTH)
            bd_image0_reserved:       .word 0x00000000
        /* IPL image */
        bd_image1_base:
            bd_image1_src_ptr:        .quad (_ipl_code_start - dtb_base) /* Image1 source 64 bits Address of image in boot media */
            bd_image1_dest_ptr:       .quad (_ipl_code_start)       /* Image1 destination 64 bits Absolute address of image in system memory */
            bd_image1_entry_ptr:      .quad (_ipl_code_start)       /* Image1 entry 64 bits Entry point for image in system memory */
            bd_image1_size:           .word (__ipl_code_size)
            bd_image1_reserved:       .word 0x00000000
    #if (IMX_CM7_CORE_BOOT == 1)
        /* Cortex-M7 image */
        bd_image2_base:
            bd_image2_src_ptr:        .quad (__cm7_begin - dtb_base) /* Image2 source 64 bits Address of image in boot media */
            bd_image2_dest_ptr:       .quad (IMX_CM7_CORE_FW_LOAD_ADDR) /* Image2 destination 64 bits Absolute address of image in system memory */
            bd_image2_entry_ptr:      .quad (IMX_CM7_CORE_FW_LOAD_ADDR) /* Image2 entry 64 bits Entry point for image in system memory */
            bd_image2_size:           .word (__cm7_code_size)
            bd_image2_reserved:       .word 0x00000000
    #endif
    bd_images_end:
#endif

#if !defined(IMX_SPL_BOOT)
    /* **************************** */
    /* ARM Trusted Firmware section */
    /* **************************** */
    .section ".ArmTrustedFW", "a"
        .incbin IMX_ARM_TRUSTED_FW_BINARY
#endif

#if (!defined(IMX_SPL_BOOT) && (IMX_CM7_CORE_BOOT == 1))
    /* ********************************* */
    /* Cortex-M4 core Firmware section */
    /* ********************************* */
    .section ".CortexM7_Fw", "a"
        .incbin IMX_CM7_CORE_FIRMWARE
#endif

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/image_header.S $ $Rev: 958436 $";
.previous
#endif
#endif
