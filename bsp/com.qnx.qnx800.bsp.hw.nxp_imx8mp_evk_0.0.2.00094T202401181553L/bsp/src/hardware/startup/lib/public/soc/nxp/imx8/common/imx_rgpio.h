/*
 * Copyright 2023, BlackBerry Limited. All rights reserved.
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

#ifndef IMX_RGPIO_H_
#define IMX_RGPIO_H_

/* RGPIO Registers, offset from base address */
#define IMX_RGPIO_VERID                         0x0000      /* Version ID Register */
#define IMX_RGPIO_PARAM                         0x0004      /* Parameter Register */
#define IMX_RGPIO_LOCK                          0x000C      /* Lock Register */
#define IMX_RGPIO_PCNS                          0x0010      /* Pin Control Non-Secure */
#define IMX_RGPIO_ICNS                          0x0014      /* Interrupt Control Non-Secure */
#define IMX_RGPIO_PCNP                          0x0018      /* Pin Control Non-Privilege */
#define IMX_RGPIO_ICNP                          0x001C      /* Interrupt Control Non-Privilege */
#define IMX_RGPIO_PDOR                          0x0040      /* Port Data Output Register */
#define IMX_RGPIO_PSOR                          0x0044      /* Port Set Output Register */
#define IMX_RGPIO_PCOR                          0x0048      /* Port Clear Output Register */
#define IMX_RGPIO_PTOR                          0x004C      /* Port Toggle Output Register */
#define IMX_RGPIO_PDIR                          0x0050      /* Port Data Input Register */
#define IMX_RGPIO_PDDR                          0x0054      /* Port Data Direction Register */
#define IMX_RGPIO_PIDR                          0x0058      /* Port Input Disable Register */
#define IMX_RGPIO_PDR(x)                        (0x0060 + (x)) /* Pin Data Register */
#define IMX_RGPIO_ICR(x)                        (0x0080 + ((x) * 4)) /* Interrupt Control Register */
#define IMX_RGPIO_GICLR                         0x0100      /* Global Interrupt Control Low Register */
#define IMX_RGPIO_GICHR                         0x0104      /* Global Interrupt Control High Register */
#define IMX_RGPIO_ISFR(x)                       (0x0120 + ((x) * 4)) /* Interrupt Status Flag Register */

/*
 * Version ID Register (VERID) bits *
 */
#define IMX_RGPIO_VERID_FEATURE_MASK                 (0xFFFFU)
#define IMX_RGPIO_VERID_FEATURE_SHIFT                (0U)
#define IMX_RGPIO_VERID_MINOR_MASK                   (0xFF0000U)
#define IMX_RGPIO_VERID_MINOR_SHIFT                  (16U)
#define IMX_RGPIO_VERID_MAJOR_MASK                   (0xFF000000U)
#define IMX_RGPIO_VERID_MAJOR_SHIFT                  (24U)

/*
 * Parameter Register (PARAM) bits *
 */
#define IMX_RGPIO_PARAM_IRQNUM_MASK                  (0xFU)
#define IMX_RGPIO_PARAM_IRQNUM_SHIFT                 (0U)

/*
 * Lock Register (LOCK) bits *
 */
#define IMX_RGPIO_LOCK_PCNS_MASK                     (0x1U)
#define IMX_RGPIO_LOCK_PCNS_SHIFT                    (0U)
#define IMX_RGPIO_LOCK_ICNS_MASK                     (0x2U)
#define IMX_RGPIO_LOCK_ICNS_SHIFT                    (1U)
#define IMX_RGPIO_LOCK_PCNP_MASK                     (0x4U)
#define IMX_RGPIO_LOCK_PCNP_SHIFT                    (2U)
#define IMX_RGPIO_LOCK_ICNP_MASK                     (0x8U)
#define IMX_RGPIO_LOCK_ICNP_SHIFT                    (3U

/*
 * Interrupt Control Non-Secure (ICNS) bits *
 */
#define IMX_RGPIO_ICNS_NSE0_MASK                     (0x1U)
#define IMX_RGPIO_ICNS_NSE0_SHIFT                    (0U)
#define IMX_RGPIO_ICNS_NSE1_MASK                     (0x2)
#define IMX_RGPIO_ICNS_NSE1_SHIFT                    (1U)

/*
 * Pin Data Register a (PDR) bits *
 */
#define IMX_RGPIO_PDR_PD_MASK                        (0x1U)
#define IMX_RGPIO_PDR_PD_SHIFT                       (0U)

/*
 * Interrupt Control Register (ICR) bits *
 */
#define IMX_RGPIO_ICR_IRQC_MASK                      (0xF0000U)
#define IMX_RGPIO_ICR_IRQC_SHIFT                     (16U)
#define IMX_RGPIO_ICR_IRQC(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_RGPIO_ICR_IRQC_SHIFT)) & IMX_RGPIO_ICR_IRQC_MASK)
#define IMX_RGPIO_ICR_IRQS_MASK                      (0x100000U)
#define IMX_RGPIO_ICR_IRQS_SHIFT                     (20U)
#define IMX_RGPIO_ICR_LK_MASK                        (0x800000U)
#define IMX_RGPIO_ICR_LK_SHIFT                       (23U)
#define IMX_RGPIO_ICR_ISF_MASK                       (0x1000000U)
#define IMX_RGPIO_ICR_ISF_SHIFT                      (24U)

#define IMX_RGPIO_ICR_IRQC_DISABLED                          (0x0U)
#define IMX_RGPIO_ICR_IRQC_IFS_DMA_ON_RISING_EDGE            (0x1U)
#define IMX_RGPIO_ICR_IRQC_IFS_DMA_ON_FALLING_EDGE           (0x2U)
#define IMX_RGPIO_ICR_IRQC_IFS_DMA_ON_EITHER_EDGE            (0x3U)
#define IMX_RGPIO_ICR_IRQC_IFS_ON_RISING_EDGE                (0x5U)
#define IMX_RGPIO_ICR_IRQC_IFS_ON_FALLING_EDGE               (0x6U)
#define IMX_RGPIO_ICR_IRQC_IFS_ON_EITHER_EDGE                (0x7U)
#define IMX_RGPIO_ICR_IRQC_IFS_INTERRUPT_ON_LOGIC_LOW        (0x8U)
#define IMX_RGPIO_ICR_IRQC_IFS_INTERRUPT_ON_RISING_EDGE      (0x9U)
#define IMX_RGPIO_ICR_IRQC_IFS_INTERRUPT_ON_FALLING_EDGE     (0xAU)
#define IMX_RGPIO_ICR_IRQC_IFS_INTERRUPT_ON_EITHER_EDGE      (0xBU)
#define IMX_RGPIO_ICR_IRQC_IFS_INTERRUPT_ON_LOGIC_HIGH       (0xCU)

#endif /* IMX_RGPIO_H_ */

