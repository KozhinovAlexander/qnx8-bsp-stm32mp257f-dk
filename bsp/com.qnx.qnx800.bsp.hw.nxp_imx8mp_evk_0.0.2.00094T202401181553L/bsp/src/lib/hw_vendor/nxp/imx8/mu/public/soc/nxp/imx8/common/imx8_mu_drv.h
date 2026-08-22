/*
 * $QNXLicenseC:
 * Copyright 2018-2019 NXP
 * Copyright 2018,2023, BlackBerry Limited.
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

#ifndef IMX8_MU_DRV_H_
#define IMX8_MU_DRV_H_

#include <pthread.h>
#include <soc/nxp/imx8/common/imx_mu.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _imx_mu {
    uint32_t  pbase;                /**< Physical base address */
    uintptr_t vbase;                /**< Virtual base address */
    int intr;                       /**< Interrupt event number from reference manual */
    int iid;                        /**< Interrupt event ID returned by InterruptAttachEvent() */
    struct sigevent intrevent;      /**< sigevent structure which is delivered when interrupt occurs */
    pthread_mutex_t tx_lock[4];     /**< TX locks */
    pthread_mutex_t rx_lock[4];     /**< RX locks */
    pthread_mutex_t gp_lock[4];     /**< GP locks */
} imx_mu_t;

/** Flags for imx_mu_send(), imx_mu_read() and imx_mu_waitfor_gpint_request(). */
#define IMX_MU_FLAG_BLOCK           1   /** This flag disables interrupt mode. */
#define IMX_MU_FLAG_LOG             2   /** This flag enables errors logging */

/** These MU flags maps on ASR register bitfields */
typedef enum _imx_mu_flags {
    imx_mu_flag0            = (1 << IMX_MU_xSR_F_SHIFT),    /**< Processor flag reflects value written to xCR F0 bit */
    imx_mu_flag1            = (2 << IMX_MU_xSR_F_SHIFT),    /**< Processor flag reflects value written to xCR F1 bit */
    imx_mu_flag2            = (4 << IMX_MU_xSR_F_SHIFT),    /**< Processor flag reflects value written to xCR F2 bit */
    imx_mu_event_pending    = IMX_MU_xSR_EP_MASK,           /**< Processor event pending EP */
    imx_mu_reset_state      = IMX_MU_xSR_xRS_MASK,          /**< The processor on other side of the MU is in reset ARS/BRS */
    imx_mu_flags_updating   = IMX_MU_xSR_FUP_MASK,          /**< Processor flags update pending FUP */
    imx_mu_cpu_out_of_reset = IMX_MU_ASR_BRDIP_MASK,        /**< Signals that processor on other side is out of reset */
    imx_mu_tx0_empty        = (8 << IMX_MU_xSR_TE_SHIFT),   /**< TX0 register empty */
    imx_mu_tx1_empty        = (4 << IMX_MU_xSR_TE_SHIFT),   /**< TX1 register empty */
    imx_mu_tx2_empty        = (2 << IMX_MU_xSR_TE_SHIFT),   /**< TX2 register empty */
    imx_mu_tx3_empty        = (1 << IMX_MU_xSR_TE_SHIFT),   /**< TX3 register empty */
    imx_mu_rx0_full         = (8 << IMX_MU_xSR_RF_SHIFT),   /**< RX0 register full */
    imx_mu_rx1_full         = (4 << IMX_MU_xSR_RF_SHIFT),   /**< RX1 register full */
    imx_mu_rx2_full         = (2 << IMX_MU_xSR_RF_SHIFT),   /**< RX2 register full */
    imx_mu_rx3_full         = (1 << IMX_MU_xSR_RF_SHIFT),   /**< RX3 register full */
    imx_mu_gen_int0         = (1 << IMX_MU_xSR_GIP_SHIFT),  /**< General interrupt request GIP0 */
    imx_mu_gen_int1         = (2 << IMX_MU_xSR_GIP_SHIFT),  /**< General interrupt request GIP1 */
    imx_mu_gen_int2         = (4 << IMX_MU_xSR_GIP_SHIFT),  /**< General interrupt request GIP2 */
    imx_mu_gen_int3         = (8 << IMX_MU_xSR_GIP_SHIFT),  /**< General interrupt request GIP3 */
} imx_mu_flags_t;

/** Basic API supports both blocking and interrupt mode via InterruptWait call. */
int      imx_mu_send(void *handle, uint32_t index, uint32_t val, uint32_t timeout, uint32_t flags);
int      imx_mu_read(void *handle, uint32_t index, uint32_t *val, uint32_t timeout, uint32_t flags);
int      imx_mu_set_flag(void *handle, uint32_t index, uint32_t set);
uint32_t imx_mu_get_flag(void *handle, uint32_t index);
int      imx_mu_set_gpint_request(void *handle, uint32_t index);
int      imx_mu_waitfor_gpint_request(void *handle, uint32_t index, uint32_t timeout, uint32_t flags);
void    *imx_mu_init(uint32_t base, int32_t intr);
int      imx_mu_deinit(void *handle);
void     imx_mu_reset(void * const handle);
int      imx_mu_waitfor_reset_complete(void * const handle, uint32_t timeout);


/** Extension to provide direct access to MU registers. User can implement own MU IRQ handler to control MU general interrupts
 * for event based communication. In such case user should call imx_mu_init(base, 0) to allocate MU handle with IRQ disabled.
 */
uint32_t imx_mu_read_status_reg( void  * const handle);
/* This functions expose direct access to ACR/BCR registers and possible conflict may occur if used together with imx_mu_send() or imx_mu_read(). */
void     imx_mu_write_status_reg(void * const handle, uint32_t const value);
uint32_t imx_mu_read_ctrl_reg(void * const handle);
void     imx_mu_write_ctrl_reg(void * const handle, uint32_t const value);
/* Note these functions expose direct access to TX,RX registers and should not be used together with imx_mu_send() or imx_mu_read() */
void     imx_mu_write_tx_reg(void * const handle, const uint32_t index, const uint32_t value);
uint32_t imx_mu_read_rx_reg(void * const handle, const uint32_t index);

#endif /* IMX8_MU_DRV_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/hw_vendor/nxp/imx8/mu/public/soc/nxp/imx8/common/imx8_mu_drv.h $ $Rev: 980950 $")
#endif
