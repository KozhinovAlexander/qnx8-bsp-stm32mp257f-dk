/*
 * Copyright (c) 2022-2023, BlackBerry Limited.
 * Copyright 2019, 2022 NXP
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
#include <soc/nxp/imx8/common/imx_usb3.h>
#include <imx_startup.h>
#include "board.h"

/**
 * i.MX startup source file.
 *
 * @file       imx_init_usb.c
 * @addtogroup startup
 * @{
 */

/* USB MIX offset */
#define USBMIX_PHY_OFFSET                   0xF0040

/* DWC3 register and bit-fields definition */
#define DWC3_GHWPARAMS1                     0xC144

#define DWC3_GSNPSID                        0xC120
#define DWC3_GCTL                           0xC110
    #define DWC3_GCTL_PWRDNSCALE_SHIFT      19
    #define DWC3_GCTL_PWRDNSCALE_MASK       (0x1FFF << 19)
    #define DWC3_GCTL_U2RSTECN_MASK         (1 << 16)
    #define DWC3_GCTL_PRTCAPDIR(n)          ((n) << 12)
       #define DWC3_GCTL_PRTCAP_HOST        1
       #define DWC3_GCTL_PRTCAP_DEVICE      2
       #define DWC3_GCTL_PRTCAP_OTG         3
    #define DWC3_GCTL_CORESOFTRESET_MASK    (1 << 11)
    #define DWC3_GCTL_SCALEDOWN(n)          ((n) << 4)
    #define DWC3_GCTL_SCALEDOWN_MASK        DWC3_GCTL_SCALEDOWN(3)
    #define DWC3_GCTL_DISSCRAMBLE_MASK      (1 << 3)
    #define DWC3_GCTL_DSBLCLKGTNG_MASK      (1 << 0)

#define DWC3_GUSB2PHYCFG                    0xC200
    #define DWC3_GUSB2PHYCFG_PHYSOFTRST_MASK         (1 << 31)
    #define DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS_MASK  (1 << 30)
    #define DWC3_GUSB2PHYCFG_ENBLSLPM_MASK           (1 << 8)
    #define DWC3_GUSB2PHYCFG_SUSPHY_MASK             (1 << 6)
    #define DWC3_GUSB2PHYCFG_PHYIF_MASK              (1 << 3)

#define DWC3_GUSB3PIPECTL                   0xC2C0
    #define DWC3_GUSB3PIPECTL_PHYSOFTRST_MASK        (1 << 31)

#define DWC3_GFLADJ                         0xC630
    #define GFLADJ_30MHZ_REG_SEL            (1 << 7)
    #define GFLADJ_30MHZ(n)                 ((n) & 0x3f)
    #define GFLADJ_30MHZ_DEFAULT            0x20

/* USB PHYx registers and bit-fields definition */
#define USB_PHY_CTRL0                       0x0
    #define USB_PHY_CTRL0_REF_SSP_EN_MASK       (1 << 2)

#define USB_PHY_CTRL1                       0x4
    #define USB_PHY_CTRL1_RESET_MASK            (1 << 0)
    #define USB_PHY_CTRL1_COMMONONN_MASK        (1 << 1)
    #define USB_PHY_CTRL1_ATERESET_MASK         (1 << 3)
    #define USB_PHY_CTRL1_VDATSRCENB0_MASK      (1 << 19)
   #define USB_PHY_CTRL1_VDATDETENB0_MASK       (1 << 20)

#define USB_PHY_CTRL2                       0x8
    #define USB_PHY_CTRL2_TXENABLEN0_MASK       (1 << 8)

#define USB_PHY_CTRL6                       0x18

#define HSIO_GPR_REG_0                              (0x32F10000U)
#define HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN_SHIFT    (1)
#define HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN          (0x1U << HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN_SHIFT)

/**
 * USB PHY initialization.
 *
 * @param base USB controller base address.
 */
static void imx8m_usb_phy_init(uint32_t base)
{
    uint32_t reg;

    /* enable usb clock via hsio gpr */
    reg = in32(HSIO_GPR_REG_0);
    reg |= HSIO_GPR_REG_0_USB_CLOCK_MODULE_EN;
    out32(HSIO_GPR_REG_0, reg);

    /* USB3.0 PHY signal fsel for 24M ref */
    reg = in32(base + USBMIX_PHY_OFFSET +  USB_PHY_CTRL0);
    reg = (reg & 0xFFFFF81F) | (0x2A << 5);
    out32(base + USBMIX_PHY_OFFSET +  USB_PHY_CTRL0, reg);

    reg = in32(base + USBMIX_PHY_OFFSET +  USB_PHY_CTRL6);
    reg &= ~(0x01);
    out32(base + USBMIX_PHY_OFFSET +  USB_PHY_CTRL6, reg);

    reg = in32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL1);
    reg &= ~(USB_PHY_CTRL1_VDATSRCENB0_MASK | USB_PHY_CTRL1_VDATDETENB0_MASK);
    reg |= USB_PHY_CTRL1_RESET_MASK | USB_PHY_CTRL1_ATERESET_MASK;
    out32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL1, reg);

    reg = in32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL0);
    reg |= USB_PHY_CTRL0_REF_SSP_EN_MASK;
    out32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL0, reg);

    reg = in32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL2);
    reg |= USB_PHY_CTRL2_TXENABLEN0_MASK;
    out32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL2, reg);

    reg = in32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL1);
    reg &= ~(USB_PHY_CTRL1_RESET_MASK | USB_PHY_CTRL1_ATERESET_MASK);
    out32(base + USBMIX_PHY_OFFSET + USB_PHY_CTRL1, reg);
}

/**
 * DWC3 PHY initialization.
 *
 * @param base USB controller base address.
 */
static void dwc3_phy_reset(uint32_t base)
{
    uint32_t reg;

    /* Before Resetting PHY, put Core in Reset */
    reg = in32(base + DWC3_GCTL);
    reg |= DWC3_GCTL_CORESOFTRESET_MASK;
    out32(base + DWC3_GCTL, reg);

    /* Assert USB3 PHY reset */
    reg = in32(base + DWC3_GUSB3PIPECTL);
    reg |= DWC3_GUSB3PIPECTL_PHYSOFTRST_MASK;
    out32(base + DWC3_GUSB3PIPECTL, reg);

    /* Assert USB2 PHY reset */
    reg = in32(base + DWC3_GUSB2PHYCFG);
    reg |= DWC3_GUSB2PHYCFG_PHYSOFTRST_MASK;
    out32(base + DWC3_GUSB2PHYCFG, reg);

    imx_usleep(100 * 1000);

    /* Clear USB3 PHY reset */
    reg = in32(base + DWC3_GUSB3PIPECTL);
    reg &= ~DWC3_GUSB3PIPECTL_PHYSOFTRST_MASK;
    out32(base + DWC3_GUSB3PIPECTL, reg);

    /* Clear USB2 PHY reset */
    reg = in32(base + DWC3_GUSB2PHYCFG);
    reg &= ~DWC3_GUSB2PHYCFG_PHYSOFTRST_MASK;
    out32(base + DWC3_GUSB2PHYCFG, reg);

    imx_usleep(100 * 1000);

    /* After PHYs are stable we can take Core out of reset state */
    reg = in32(base + DWC3_GCTL);
    reg &= ~DWC3_GCTL_CORESOFTRESET_MASK;
    out32(base + DWC3_GCTL, reg);
}

/**
 * DWC3 core initialization.
 *
 * @param base USB controller base address.
 */
static void dwc3_core_init(uint32_t base)
{
    uint32_t reg;
    uint32_t revision;

    revision = in32(base + DWC3_GSNPSID);

    if ((revision & 0xFFFF0000) != 0x55330000) {
        kprintf("This is not a DesignWare USB3 DRD Core %x\n", revision);
    }

    dwc3_phy_reset(base);

    reg = in32(base + DWC3_GCTL);
    reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
    reg &= ~DWC3_GCTL_DISSCRAMBLE_MASK;
    reg &= ~DWC3_GCTL_DSBLCLKGTNG_MASK;
    out32(base + DWC3_GCTL, reg);
}

/**
 * Set suspend_clk to 32KHz.
 *
 * @param base USB controller base address.
 */
static void imx8m_xhci_set_suspend_clk(uint32_t base)
{
    uint32_t reg;

    /* Set suspend_clk to be 32KHz */
    reg = in32(base + DWC3_GCTL);
    reg &= ~DWC3_GCTL_PWRDNSCALE_MASK;
    reg |= 2 << DWC3_GCTL_PWRDNSCALE_SHIFT;

    out32(base + DWC3_GCTL, reg);
}

/**
 * This function configures controller core and calls controller
 * initialization method for OTG1, OTG2 controller.
 */
void imx_usb3_otg_host_init(void)
{
    uint32_t reg;

    imx8m_usb_phy_init(IMX_USB3_OTG1_BASE);
    imx8m_usb_phy_init(IMX_USB3_OTG2_BASE);

    dwc3_core_init(IMX_USB3_OTG1_BASE);
    dwc3_core_init(IMX_USB3_OTG2_BASE);

    imx8m_xhci_set_suspend_clk(IMX_USB3_OTG1_BASE);
    imx8m_xhci_set_suspend_clk(IMX_USB3_OTG2_BASE);

    /* Set DWC3 core to Host Mode for OTG1 */
    reg = in32(IMX_USB3_OTG1_BASE + DWC3_GCTL);
    reg &= ~DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG);
    reg |= DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST);
    out32(IMX_USB3_OTG1_BASE + DWC3_GCTL, reg);

    /* Set DWC3 core to Host Mode for OTG2 */
    reg = in32(IMX_USB3_OTG2_BASE + DWC3_GCTL);
    reg &= ~DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG);
    reg |= DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST);
    out32(IMX_USB3_OTG2_BASE + DWC3_GCTL, reg);

    /* Set GFLADJ_30MHZ as 20h as per XHCI spec default value */
    reg = in32(IMX_USB3_OTG1_BASE + DWC3_GFLADJ);
    reg |= GFLADJ_30MHZ_REG_SEL | GFLADJ_30MHZ(GFLADJ_30MHZ_DEFAULT);
    out32(IMX_USB3_OTG1_BASE + DWC3_GFLADJ, reg);

    /* Set GFLADJ_30MHZ as 20h as per XHCI spec default value */
    reg = in32(IMX_USB3_OTG2_BASE + DWC3_GFLADJ);
    reg |= GFLADJ_30MHZ_REG_SEL | GFLADJ_30MHZ(GFLADJ_30MHZ_DEFAULT);
    out32(IMX_USB3_OTG2_BASE + DWC3_GFLADJ, reg);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_usb.c $ $Rev: 984580 $")
#endif
