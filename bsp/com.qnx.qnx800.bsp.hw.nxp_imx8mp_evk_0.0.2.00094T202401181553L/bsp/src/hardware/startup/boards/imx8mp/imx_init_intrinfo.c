/*
 * Copyright (c) 2014, 2022-2023, BlackBerry Limited.
 * Copyright 2018-2019, 2023 NXP
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

/*
 * i.MX General Interrupt Controller support.
 */

#include <startup.h>
#include "imx_init_intrinfo.h"
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <stdbool.h>
#include <aarch64/gic.h>
#include <soc/nxp/imx8/common/imx_pcie.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_init_intrinfo.c
 * @addtogroup startup
 * @{
 *
 * <h2 class="groupheader"> Interrupt vector mapping</h2>
 * <div class="memitem">
 * <div class="memproto">
 * Interrupt vector:<br/>
 * </div>
 *  <div class="memdoc">
 *   <UL>
 *    <li> 32 - 160: ARM General Interrupt Controller (same IRQs as defined in corresponding reference manual)
 *    <li>544 - 575: GPIO 0 interrupts, device GPIO0[0 - 31]
 *    <li>576 - 607: GPIO 1 interrupts, device GPIO1[0 - 31]
 *    <li>608 - 639: GPIO 2 interrupts, device GPIO2[0 - 31]
 *    <li>640 - 671: GPIO 3 interrupts, device GPIO3[0 - 31]
 *    <li>672 - 703: GPIO 4 interrupts, device GPIO4[0 - 31]
 *    <li>704 - 735: GPIO 5 interrupts, device GPIO5[0 - 31]
 *    <li>4096 - 4351: PCIe MSI interrupts, device MSI[0 - 255]
 *   </UL>
 *  </div>
 * </div>
 */

static paddr_t imx_gpio1_base = IMX_GPIO1_BASE;
static paddr_t imx_gpio2_base = IMX_GPIO2_BASE;
static paddr_t imx_gpio3_base = IMX_GPIO3_BASE;
static paddr_t imx_gpio4_base = IMX_GPIO4_BASE;
static paddr_t imx_gpio5_base = IMX_GPIO5_BASE;
static paddr_t imx_irq_steer_hdmi_base = IMX_IRQ_STEER_HDMI_BASE;

/*
 *
 * Prioritizing them within the callouts will be done as follows
 *
 * highest priority - bit 0 in set 0 (return 0)
 *     :
 *               - bit 31 in set 0 (return 31)
 *               - bit 0 in set 1 (return 32)
 *     :
 *               - bit 31 in set 6 (return 223)
 *               - bit 0 in set 7 (return 224)
 *     :
 * 2nd lowest priority - bit 31 in set 7 (return 255)
 * lowest priority - (if no bits set in any of the 8 status registers) return 256 corresponding to PCI INTA
 *
 * This prioritization scheme ensures that MSI/MSI-X vectors are assigned in highest
 * priority order on a FIFO basis by the PCI server
 *
 */
static paddr_t imx_pcie_msi_base = (IMX_PCIE1_BASE + IMX_PCIE_PL_MSICIn_ENB);


/*
 * GICv3 support code
 */
const static struct startup_intrinfo pci_gpio[] = {
    /**< PCIe1 MSI/MSI-X interrupts (256) starting at 0x1000) */
    {   .vector_base     = 0x1000,              /* Vector base (MSI/MSI-X 0x1000 - 0x10FF) */
        .num_vectors     = 256,                 /* 256 MSI's */
        .cascade_vector  = IMX_PCIE1_MSI,       /* MSI vector number */
        .cpu_intr_base   = 0,                   /* CPU vector base (MSI/MSI-X from 0 - 255) */
        .cpu_intr_stride = 0,
        .flags           = INTR_FLAG_MSI,
        .id              = { .genflags = INTR_GENFLAG_NOGLITCH, .size = 0, .rtn = &interrupt_id_imx_msi },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_msi },
        .mask            = &interrupt_mask_imx_msi,
        .unmask          = &interrupt_unmask_imx_msi,
        .config          = 0,
        .patch_data      = &imx_pcie_msi_base,
    },
    /**< GPIO 1 low interrupt vectors 544 - 559 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(1, 0),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO1_LOW_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_low },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_low },
        .mask            = &interrupt_mask_imx_gpio_low,
        .unmask          = &interrupt_unmask_imx_gpio_low,
        .config          = 0,
        .patch_data      = &imx_gpio1_base,
    },
    /**< GPIO 1 high interrupt vectors 560 - 575 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(1, 16),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO1_HIGH_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_high },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_high },
        .mask            = &interrupt_mask_imx_gpio_high,
        .unmask          = &interrupt_unmask_imx_gpio_high,
        .config          = 0,
        .patch_data      = &imx_gpio1_base,
    },
    /**< GPIO 2 low interrupt vectors 576 - 591 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(2, 0),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO2_LOW_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_low },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_low },
        .mask            = &interrupt_mask_imx_gpio_low,
        .unmask          = &interrupt_unmask_imx_gpio_low,
        .config          = 0,
        .patch_data      = &imx_gpio2_base,
    },
    /**< GPIO 2 high interrupt vectors 592 - 607 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(2, 16),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO2_HIGH_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_high },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_high },
        .mask            = &interrupt_mask_imx_gpio_high,
        .unmask          = &interrupt_unmask_imx_gpio_high,
        .config          = 0,
        .patch_data      = &imx_gpio2_base,
    },
    /**< GPIO 3 low interrupt vectors 608 - 623 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(3, 0),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO3_LOW_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_low },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_low },
        .mask            = &interrupt_mask_imx_gpio_low,
        .unmask          = &interrupt_unmask_imx_gpio_low,
        .config          = 0,
        .patch_data      = &imx_gpio3_base,
    },
    /**< GPIO 3 high interrupt vectors 624 - 639 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(3, 16),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO3_HIGH_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_high },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_high },
        .mask            = &interrupt_mask_imx_gpio_high,
        .unmask          = &interrupt_unmask_imx_gpio_high,
        .config          = 0,
        .patch_data      = &imx_gpio3_base,
    },
    /**< GPIO 4 low interrupt vectors 640 - 655 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(4, 0),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO4_LOW_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_low },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_low },
        .mask            = &interrupt_mask_imx_gpio_low,
        .unmask          = &interrupt_unmask_imx_gpio_low,
        .config          = 0,
        .patch_data      = &imx_gpio4_base,
    },
    /**< GPIO 4 high interrupt vectors 656 - 671 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(4, 16),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO4_HIGH_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_high },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK,  0, .rtn = &interrupt_eoi_imx_gpio_high },
        .mask            = &interrupt_mask_imx_gpio_high,
        .unmask          = &interrupt_unmask_imx_gpio_high,
        .config          = 0,
        .patch_data      = &imx_gpio4_base,
    },
    /**< GPIO 5 low interrupt vectors 672 - 687 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(5, 0),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO5_LOW_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_low },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_low },
        .mask            = &interrupt_mask_imx_gpio_low,
        .unmask          = &interrupt_unmask_imx_gpio_low,
        .config          = 0,
        .patch_data      = &imx_gpio5_base,
    },
    /**< GPIO 5 high interrupt vectors 688 - 703 */
    {
        .vector_base     = IMX_GPIO_CASCADE_IRQ(5, 16),
        .num_vectors     = 16,
        .cascade_vector  = IMX_GPIO5_HIGH_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_gpio_high },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_gpio_high },
        .mask            = &interrupt_mask_imx_gpio_high,
        .unmask          = &interrupt_unmask_imx_gpio_high,
        .config          = 0,
        .patch_data      = &imx_gpio5_base,
    },
    /**< HDMI 800 - 831 */
    {
        .vector_base     = 800,
        .num_vectors     = 32,
        .cascade_vector  = IMX_HDMI_IRQ,
        .cpu_intr_base   = 0,
        .cpu_intr_stride = 0,
        .flags           = 0,
        .id              = { .genflags = 0, .size = 0, .rtn = &interrupt_id_imx_irqsteer },
        .eoi             = { .genflags = INTR_GENFLAG_LOAD_INTRMASK, .size = 0, .rtn = &interrupt_eoi_imx_irqsteer },
        .mask            = &interrupt_mask_imx_irqsteer,
        .unmask          = &interrupt_unmask_imx_irqsteer,
        .config          = 0,
        .patch_data      = &imx_irq_steer_hdmi_base,
    }
};

/**
 * Initialize Generic Interrupt Controller (GIC).
 */
void init_intrinfo(void)
{
    /* Initialize GIC  */
    gic_v3_set_paddr(IMX_GIC_GICD_BASE, IMX_GIC_GICR_BASE, NULL_PADDR);
    gic_v3_use_mm_reg_callouts(IMX_GIC_GICC_BASE, false);
    gic_v3_initialize();
    /* Add the interrupt callouts */
    add_interrupt_array(pci_gpio, sizeof(pci_gpio));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_intrinfo.c $ $Rev: 984580 $")
#endif
