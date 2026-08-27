/*
 * $QNXLicenseC:
 * Copyright 2022 BlackBerry Limited.
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
 * $
 */

#ifndef IMX_OCOTP_H_
#define IMX_OCOTP_H_

/* OCOTP Registers, offset from base address */
#define IMX_HW_OCOTP_CTRL                       0x000       /* OTP Controller Control Register */
#define IMX_HW_OCOTP_CTRL_SET                   0x004       /* OTP Controller Control Register */
#define IMX_HW_OCOTP_CTRL_CLR                   0x008       /* OTP Controller Control Register */
#define IMX_HW_OCOTP_CTRL_TOG                   0x00C       /* OTP Controller Control Register */
#define IMX_HW_OCOTP_TIMING                     0x010       /* OTP Controller Timing Register */
#define IMX_HW_OCOTP_DATA                       0x020       /* OTP Controller Write Data Register */
#define IMX_HW_OCOTP_READ_CTRL                  0x030       /* OTP Controller Write Data Register */
#define IMX_HW_OCOTP_READ_FUSE_DATA             0x040       /* OTP Controller Read Data Register */
#define IMX_HW_OCOTP_VERSION                    0x090       /* OTP Controller Version Register */
#define IMX_HW_OCOTP_LOCK0                      0x400       /* Value of OTP Bank0 Word0 */
#define IMX_HW_OCOTP_LOCK1                      0x410       /* Value of OTP Bank0 Word1 */
#define IMX_HW_OCOTP_TESTER0                    0x410       /* Value of OTP Bank0 Word1 */
#define IMX_HW_OCOTP_TESTER1                    0x420       /* Value of OTP Bank0 Word2 */
#define IMX_HW_OCOTP_TESTER2                    0x430       /* Value of OTP Bank0 Word3 */
#define IMX_HW_OCOTP_TESTER3                    0x440       /* Value of OTP Bank1 Word0 */
#define IMX_HW_OCOTP_TESTER4                    0x450       /* Value of OTP Bank1 Word1 */
#define IMX_HW_HW_OCOTP_GP4                     0x460       /* Value of OTP Bank1 Word2 */
#define IMX_HW_OCOTP_BOOT_CFG0                  0x470       /* Value of OTP Bank1 Word3 */
#define IMX_HW_OCOTP_BOOT_CFG1                  0x480       /* Value of OTP Bank2 Word0 */
#define IMX_HW_OCOTP_BOOT_CFG2                  0x490       /* Value of OTP Bank2 Word1 */
#define IMX_HW_OCOTP_BOOT_CFG3                  0x4A0       /* Value of OTP Bank2 Word2 */
#define IMX_HW_OCOTP_BOOT_CFG4                  0x4B0       /* Value of OTP Bank2 Word3 */
#define IMX_HW_OCOTP_SRK_HASH_0                 0x580       /* SRK key Word0 */
#define IMX_HW_OCOTP_SRK_HASH_1                 0x584       /* SRK key Word1 */
#define IMX_HW_OCOTP_SRK_HASH_2                 0x588       /* SRK key Word2 */
#define IMX_HW_OCOTP_SRK_HASH_3                 0x58C       /* SRK key Word3 */
#define IMX_HW_OCOTP_SRK_HASH_4                 0x590       /* SRK key Word4 */
#define IMX_HW_OCOTP_SRK_HASH_5                 0x594       /* SRK key Word5 */
#define IMX_HW_OCOTP_SRK_HASH_6                 0x598       /* SRK key Word6 */
#define IMX_HW_OCOTP_SRK_HASH_7                 0x59C       /* SRK key Word7 */
#define IMX_HW_OCOTP_USB_ID                     0x620       /* Value of OTP Bank8 Word2 */
#define IMX_HW_OCOTP_FIELD_RETURN               0x630       /* Value of OTP Bank5 Word6 */
#define IMX_HW_OCOTP_MAC_ADDR0                  0x640       /* Value of OTP Bank9 Word0 */
#define IMX_HW_OCOTP_MAC_ADDR1                  0x650       /* Value of OTP Bank9 Word1 */
#define IMX_HW_OCOTP_MAC_ADDR2                  0x660       /* Value of OTP Bank9 Word2 */
#define IMX_HW_OCOTP_GP1_0                      0x780       /* Value of OTP Bank14 Word0 */
#define IMX_HW_OCOTP_GP1_1                      0x790       /* Value of OTP Bank14 Word1 */
#define IMX_HW_OCOTP_GP2_0                      0x7A0       /* Value of OTP Bank14 Word2 */
#define IMX_HW_OCOTP_GP2_1                      0x7B0       /* Value of OTP Bank14 Word3 */
#define IMX_HW_OCOTP_GP6_0                      0xE40       /* Value of OTP Bank41 Word0 */
#define IMX_HW_OCOTP_GP6_1                      0xE50       /* Value of OTP Bank41 Word1 */
#define IMX_HW_OCOTP_GP6_2                      0xE60       /* Value of OTP Bank41 Word2 */
#define IMX_HW_OCOTP_GP6_3                      0xE70       /* Value of OTP Bank41 Word3 */
#define IMX_HW_OCOTP_GP7_0                      0xE80       /* Value of OTP Bank42 Word0 */
#define IMX_HW_OCOTP_GP7_1                      0xE90       /* Value of OTP Bank42 Word1 */
#define IMX_HW_OCOTP_GP7_2                      0xEA0       /* Value of OTP Bank42 Word2 */
#define IMX_HW_OCOTP_GP7_3                      0xEB0       /* Value of OTP Bank42 Word3 */
#define IMX_HW_OCOTP_GP8_0                      0xEC0       /* Value of OTP Bank43 Word0 */
#define IMX_HW_OCOTP_GP8_1                      0xED0       /* Value of OTP Bank43 Word1 */
#define IMX_HW_OCOTP_GP8_2                      0xEE0       /* Value of OTP Bank43 Word2 */
#define IMX_HW_OCOTP_GP8_3                      0xEF0       /* Value of OTP Bank43 Word3 */
#define IMX_HW_OCOTP_GP9_0                      0xF00       /* Value of OTP Bank44 Word0 */
#define IMX_HW_OCOTP_GP9_1                      0xF10       /* Value of OTP Bank44 Word1 */
#define IMX_HW_OCOTP_GP9_2                      0xF20       /* Value of OTP Bank44 Word2 */
#define IMX_HW_OCOTP_GP9_3                      0xF30       /* Value of OTP Bank44 Word3 */

#endif
