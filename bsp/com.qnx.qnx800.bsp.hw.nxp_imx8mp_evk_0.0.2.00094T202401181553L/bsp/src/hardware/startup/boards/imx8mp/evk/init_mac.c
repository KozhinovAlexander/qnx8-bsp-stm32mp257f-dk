/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
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
 */

#include <startup.h>
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <soc/nxp/imx8/mp/imx_ocotp.h>
#include "mp_evk_cpu.h"
#include "board.h"

/**
 * i.MX startup source file.
 *
 * @file       startup/boards/imx8mp/evk/init_mac.c
 * @addtogroup startup
 * @{
 */

/* Offsets of the ENETs MAC address registers */
#define IMX_ENET_PALR		0xE4
#define IMX_ENET_PAUR		0xE8

#define IMX_ENET_QOS_MAC_ADDRESS0_HIGH		0x300
#define IMX_ENET_QOS_MAC_ADDRESS0_LOW		0x304

#if IMX_ENET_GET_MAC_ENABLED

/**
 * Function reads Ethernet MAC addresses from OCOTP fuses.
 * Obtained addresses are then written into ENETs peripherals.
 * For boards which do not have MAC address burned in fuses, do nothing and let iosock driver handle MAC
 */
void imx_init_enet_mac_addr(void)
{
    unsigned int  mac_low, mac_high;
    unsigned char enet1_mac_addr[6];
    unsigned char enet_qos_mac_addr[6];
    unsigned int  fuse_MAC0 = 0U;
    unsigned int  fuse_MAC1 = 0U;
    unsigned int  fuse_MAC2 = 0U;

    /* Read value from OCOTP_MAC_ADDR0, OCOTP_MAC_ADDR1 registers */
    fuse_MAC0 = in32(IMX_OCOTP_CTRL_BASE + IMX_HW_OCOTP_MAC_ADDR0);
    fuse_MAC1 = in32(IMX_OCOTP_CTRL_BASE + IMX_HW_OCOTP_MAC_ADDR1);
    fuse_MAC2 = in32(IMX_OCOTP_CTRL_BASE + IMX_HW_OCOTP_MAC_ADDR2);

    if (debug_flag) {
        kprintf("Fuse MAC0: 0x%x\n", fuse_MAC0);
        kprintf("Fuse MAC1: 0x%x\n", fuse_MAC1);
        kprintf("Fuse MAC2: 0x%x\n", fuse_MAC2);
    }

    if (fuse_MAC0) {
        /* ENET1 MAC Address settings */
        enet1_mac_addr[5] = (unsigned char)(fuse_MAC0 & 0xFF);
        enet1_mac_addr[4] = (unsigned char)((fuse_MAC0 >> 8) & 0xFF);
        enet1_mac_addr[3] = (unsigned char)((fuse_MAC0 >> 16) & 0xFF);
        enet1_mac_addr[2] = (unsigned char)((fuse_MAC0 >> 24) & 0xFF);

        enet1_mac_addr[1] = (unsigned char)(fuse_MAC1 & 0xFF);
        enet1_mac_addr[0] = (unsigned char)((fuse_MAC1 >> 8) & 0xFF);

        /* ENET_QOS MAC Address settings */
        enet_qos_mac_addr[5] = (fuse_MAC1 >> 16) & 0xFF;
        enet_qos_mac_addr[4] = (fuse_MAC1 >> 24) & 0xFF;
        enet_qos_mac_addr[3] = (fuse_MAC2) & 0xFF;
        enet_qos_mac_addr[2] = (fuse_MAC2 >> 8) & 0xFF;

        enet_qos_mac_addr[1] = (fuse_MAC2 >> 16) & 0xFF;
        enet_qos_mac_addr[0] = (fuse_MAC2 >> 24) & 0xFF;

        /* Store ENET1 MAC address into internal ENET registers  */
        mac_low = ((unsigned int)(enet1_mac_addr[0] << 24) + (unsigned int)(enet1_mac_addr[1] << 16) + (unsigned int)(enet1_mac_addr[2] << 8) + (unsigned int)(enet1_mac_addr[3]));
        out32(IMX_ENET1_BASE + IMX_ENET_PALR, mac_low);

        mac_high = (unsigned int)((enet1_mac_addr[4] << 24) + (enet1_mac_addr[5] << 16));
        out32(IMX_ENET1_BASE + IMX_ENET_PAUR, mac_high);

        /* Store ENET_QOS MAC address into internal ENET_QOS registers  */
        mac_low = ((enet_qos_mac_addr[3] << 24) + (enet_qos_mac_addr[2] << 16) + (enet_qos_mac_addr[1] << 8) + enet_qos_mac_addr[0]);
        out32(IMX_ENET_QOS_BASE + IMX_ENET_QOS_MAC_ADDRESS0_LOW, mac_low);

        mac_high = ((enet_qos_mac_addr[5] << 8) + (enet_qos_mac_addr[4]));
        out32(IMX_ENET_QOS_BASE + IMX_ENET_QOS_MAC_ADDRESS0_HIGH, mac_high);

        if (debug_flag) {
            kprintf("ENET1 MAC: %b:%b:%b:%b:%b:%b\n", enet1_mac_addr[0], enet1_mac_addr[1], enet1_mac_addr[2],
                                                      enet1_mac_addr[3], enet1_mac_addr[4], enet1_mac_addr[5]);

            kprintf("ENET_QOS MAC: %b:%b:%b:%b:%b:%b\n", enet_qos_mac_addr[0], enet_qos_mac_addr[1], enet_qos_mac_addr[2],
                                                         enet_qos_mac_addr[3], enet_qos_mac_addr[4], enet_qos_mac_addr[5]);
        }
    } else {
        if (debug_flag > 1) {
            mac_low = in32(IMX_ENET_QOS_BASE + IMX_ENET_PALR);
            mac_high = in32(IMX_ENET_QOS_BASE + IMX_ENET_PAUR);
            kprintf("ENET1 MAC: 0x%x:0x%x\n", mac_low, mac_high);

            mac_low = in32(IMX_ENET_QOS_BASE + IMX_ENET_QOS_MAC_ADDRESS0_LOW);
            mac_high = in32(IMX_ENET_QOS_BASE + IMX_ENET_QOS_MAC_ADDRESS0_HIGH);
            kprintf("ENET_QOS MAC: 0x%x:0x%x\n", mac_low, mac_high);
        }
    }
}
#endif // IMX_ENET_GET_MAC_ENABLED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/init_mac.c $ $Rev: 987026 $")
#endif
