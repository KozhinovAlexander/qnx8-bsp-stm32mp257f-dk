/*
 * Copyright (c) 2022, BlackBerry Limited.
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

#include <startup.h>
#include "imx_startup.h"
#include "board.h"
#include "imx_i2c_drv.h"


typedef struct {
        unsigned int vdd_arm;       /* VDD_ARM voltage in mV */
        unsigned int vdd_soc;       /* VDD_SOC voltage in mV */
        unsigned char arm_overdrive; /* Whether ARM cores can run in overdrive mode */
        unsigned char soc_overdrive; /* Whether subsystems powered by VDD_SOC can run in overdrive mode */
} pmic_info_t;

static pmic_info_t pmic_info = { .vdd_arm = 850U, .vdd_soc = 850U, .arm_overdrive = 0U, .soc_overdrive = 0U };

static void pmic_get_info(void) {
    imx_i2c_dev_t dev;
    unsigned char value;
    /* I2C1 base */
    dev.base = 0x30A20000;
    dev.div = 22;
    /* PCA9450 slave address */
    dev.slave = 0x25;

    init_i2c_bus(&dev);
    /* Check BUCK123_DVS register */
    if (i2c_read(&dev, 0x0C, &value) != 0) {
        kprintf("Failed to read PMIC register 0x0C\n");
        return;
    }
    /* Check whether PRESET_EN is set */
    if (value & (0x1 << 0x7)) {
        pmic_info.vdd_arm = 800U + ((value & 0x7) * 50U);
        pmic_info.vdd_soc = 800U + (((value >> 0x3) & 0x3) * 50U);
    } else {
        /* Voltage is controlled by BUCKOUT registers:
         VDD_SOC */
        if (i2c_read(&dev, 0x11, &value) != 0) {
            kprintf("Failed to read PMIC register 0x11\n");
            return;
        }
        /* Step is 12.5 mV */
        pmic_info.vdd_soc = (6000U + (value & 0x7F) * 125U) / 10U;
        /* VDD_ARM */
        if (i2c_read(&dev, 0x14, &value) != 0) {
            kprintf("Failed to read PMIC register 0x14\n");
            return;
        }
        /* Step is 12.5 mV */
        pmic_info.vdd_arm = (6000U + (value & 0x7F) * 125U) / 10U;
    }

    if (pmic_info.vdd_arm >= 900U) {
        pmic_info.arm_overdrive = 1;
    }
    if (pmic_info.vdd_soc >= 900U) {
        pmic_info.soc_overdrive = 1;
    }
    if (debug_flag) {
        kprintf("ARM_VDD: %umV (OV:%u)\nSOC_VDD: %umV (OV:%u)\n", pmic_info.vdd_arm, pmic_info.arm_overdrive,
                pmic_info.vdd_soc , pmic_info.soc_overdrive);
    }
}

unsigned char imx_get_soc_overdrive() {
    pmic_get_info();
    return pmic_info.arm_overdrive;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/pmic.c $ $Rev: 989175 $")
#endif
