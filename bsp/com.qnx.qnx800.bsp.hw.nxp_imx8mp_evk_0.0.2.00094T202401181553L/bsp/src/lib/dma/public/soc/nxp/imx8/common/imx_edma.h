/*
 * Copyright (c) 2016, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, 2020, NXP
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
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef _IMX_EDMA_H_
#define _IMX_EDMA_H_

/**
 * @file        src/lib/dma/public/hw/edma.h
 * @addtogroup  edma
 * @{
 */

#include <stdint.h>

/** Defines maximum number of TCD descriptors. The source or destination buffer consists of one or multiple fragments
 *  of varying lengths. Each fragment configures one TCD descriptor. So maximum number of fragments is equal to maximum
 *  number of TCD descriptors.
 */
#define MAX_DESCRIPTORS 256
/** eDMA driver description string */
#ifndef IMX_DMA_DESCRIPTION_STR
    #define IMX_DMA_DESCRIPTION_STR    "i.MX eDMA Controller driver"
#endif

#define IMX_DMA_DEVICE_SIZE                      0x10000
#define IMX_SUBSYSTEM_MASK                       0xFF000000

/** DMA request sources definition.
* bits: 31..31 - 1 = SW request, 0 = peripheral request
*       20..11 - eDMA channel irq number
*        6..5  - eDMA controller index
*        4..0  - request source (channel) number
*/
#define IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK  0x001FF800
#define IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT 11
#define IMX_DMA_REQUEST_DMA_NUMBER_MASK          0x00000060
#define IMX_DMA_REQUEST_DMA_NUMBER_SHIFT         5
#define IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK      0x0000001F
#define IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT     0

#define IMX_DMA_REQ_SW                         0x80000000
#define IMX_DMA_REQ_NONE                       0xFFFFFFFF
#define IMX_DMA_COMMON_IRQ                     (imx_edma_request_get_irq_number(IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK))

#define imx_edma_define_request(_edma_num_,_channel_num_,_irq_num_) ( \
        (((_irq_num_) << IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT) & IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK) | \
        (((_edma_num_) << IMX_DMA_REQUEST_DMA_NUMBER_SHIFT) & IMX_DMA_REQUEST_DMA_NUMBER_MASK) | \
        (((_channel_num_) << IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT) & IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK) \
)
#define imx_edma_request_get_irq_number(_request_)        (uint32_t)((((_request_) & IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK) >> IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT))
#define imx_edma_request_get_idx(_request_)               (uint32_t)(((_request_) & (IMX_DMA_REQUEST_DMA_NUMBER_MASK | IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK)))
#define imx_edma_request_get_dma_number(_request_)        (uint32_t)((((_request_) & IMX_DMA_REQUEST_DMA_NUMBER_MASK) >> IMX_DMA_REQUEST_DMA_NUMBER_SHIFT))
#define imx_edma_request_get_channel_number(_request_)    (uint32_t)((((_request_) & IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK) >> IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT))

/** Lowest channel number. Gathers all channels from all eDMA peripherals */
#define IMX_DMA_CH_LO                  0
/** Highest channel number. Gathers all channels from all eDMA peripherals. Also reserved channels. */
#define IMX_DMA_CH_HI                  (4 * 32)
/** Channel count. Gathers all channels from all eDMA peripherals. Also reserved channels. */
#define IMX_DMA_N_CH                   (IMX_DMA_CH_HI)
/** eDMA channel lowest priority. */
#define IMX_DMA_CH_PRIO_LO             1
/** eDMA channel highest priority. */
#define IMX_DMA_CH_PRIO_HI             7

/* Helper macros defining number of bits */
#define IMX_DMA_NBITS_8                8
#define IMX_DMA_NBITS_16               16
#define IMX_DMA_NBITS_24               24
#define IMX_DMA_NBITS_32               32
#define IMX_DMA_NBITS_64               64
#define IMX_DMA_NBITS_128              128
#define IMX_DMA_NBITS_256              256
#define IMX_DMA_NBITS_512              512

/* Define known errata */
#define IMX_EDMA_ERRATA_SBR_ATTR_SWAP  0x1

typedef unsigned imx_edma_request_source_t;

/** @} */ /* End of edma */

#endif /* _IMX_EDMA_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/public/soc/nxp/imx8/common/imx_edma.h $ $Rev: 965835 $")
#endif
