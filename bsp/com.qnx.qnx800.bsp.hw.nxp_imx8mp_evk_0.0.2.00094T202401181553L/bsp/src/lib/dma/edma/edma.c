/*
 * Copyright (c) 2016, 2022-2023, BlackBerry Limited.
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <sys/rsrcdbmgr.h>
#include <sys/mman.h>
#include <soc/nxp/imx8/common/imx_edma.h>
#include <soc/nxp/imx8/common/imx_edma_channel.h>
#include <hw/dma.h>
#include <pthread.h>
#include <hw/inout.h>
#include <soc/nxp/imx8/common/hwinfo_imx8x.h>
#include <drvr/hwinfo.h>
#include <atomic.h>
#include <fcntl.h>
#include <aarch64/inline.h>

/**
 * @file        src/lib/dma/edma/edma.c
 * @addtogroup  edma
 * @{
 */

#define IMX_EDMA_ABORT_TIMEOUT_US           1000
#define SYNC                                ({dmb();dsb();})

/* eDMA SBR ATTR masks - chip specific */
#define IMX_DMA_SBR_ATTR_WR                     (1 << 21)
#define IMX_DMA_SBR_ATTR_RD                     (1 << 22)

/** eDMA device info structure type */
typedef struct edma_device_info_s {
    uint32_t      base;
    uint32_t      chn_base;
    int           error_irq;
    int           irq;
    uint32_t      chn_num;
    unsigned int  errata;
    uint32_t      resdb_offset; /**< Channel offset added to resource database index */
} edma_device_info_t;

static edma_device_info_t  *edma_device_info = NULL;
static unsigned edma_device_count = 0;
static const unsigned edma_chn_lo = IMX_DMA_CH_LO;
static unsigned edma_chn_hi = IMX_DMA_CH_HI;

typedef struct chn_data_s {
    IMX_eDMA_Channel_t        *chn_regs_ptr;    /**< Channel TCD address  (in process address space) */
    imx_edma_request_source_t  request_source;  /**< Request source number */
    uint32_t                   chn_base;        /**< Channel base address */
    int                        chn_irq;         /**< Channel IRQ number */
    int                        chn_int_id;      /**< Value returned by InterruptAttache() method */
    int                        error_irq;       /**< eDMA error IRQ number */
    int                        error_int_id;    /**< Value returned by InterruptAttache() method */
    struct sigevent            *event;           /**< Channel event structure pointer */
    uint32_t                   attach_flags;    /**< Channel attache flags */
    uint32_t                   chn_number;      /**< Channel number */
    IMX_eDMA_TCD_t            *tcd_buffer;      /**< Address of TCD array, used for scatter/gather.
                                                     Must be in physical memory below 4 GB address space. */
    IMX_eDMA_TCD_t            *tcd_array_phy;   /**< Physical address of TCD array, used for scatter/gather.
                                                     Must be in physical memory below 4 GB address space. */
    volatile uint32_t          error_status;    /**< Channel error status */
    pthread_mutex_t            lock;            /**< Mutex for hardware and common data lock. */
    int                        fd_posix;        /**< File descriptor of posix typed memory object. */
    unsigned int               tcd_reload;      /**< Reloads TCD from buffer in every xfer_start() */
    unsigned int               bwc;             /**< Bandwidth control */
    unsigned int               eeop;            /**< End-Of-Packet processing */
    uint32_t                  *tcd_last_sda;    /**< Pointer to a buffer to store final destination address to
                                                     compute number of transfered bytes */
    uint32_t                  *tcd_last_sda_phy;/**< Physical address of tcd_last_sda buffer */
    uint32_t                   tcd_last_sda_idx;/**< Index of current SDA buffer position processed by eDMA */
    uint32_t                   sig_bytes_sent;  /**< Enables calculation of bytes sent in the IRQ handler */
    uint32_t                   tcd_cnt;         /**< Number of TCD used */
    uint8_t                    ecp;             /**< Channel can be temporarily suspended by the service request
                                                     of a higher priority channel. Disabled by default. */
    uint8_t                    dpa;             /**< Channel cannot suspend any channel, regardless of channel
                                                     priority. Disabled by default. */
    unsigned int               errata;          /**< eDMA errata. If exists. */
    uint32_t                   resdb_offset;    /**< Channel offset added to resource database index */
    struct smmu_object         *sop;            /**< SMMU object */
    volatile int               tstate;          /**< Dma IRQ thread state */
    int                        tcoid;           /**< Dma IRQ thread coid */
    int                        tprio;           /**< Dma IRQ thread priority */
    pthread_t                  tid;             /**< Dma IRQ thread id */
} chn_info_t;
/* ************************************************************************** */
/*                            PRIVATE FUNCTIONS                               */
/* ************************************************************************** */
#ifndef _DBG_INFO_
#define print_method_start_msg(...)
#define print_method_end_msg(...)
#define print_chn_info(...)
#define print_channel_regs(...)
#define print_TCD(...)
#define print_dma_transfer(...)
#define print_dma_memory(...)
#define indent_dec(...)
#define indent_inc(...)
#define print(...)

#else

#define PRINT_DMA_MEMORY_MAX_COL   8
#define PRINT_MAX_INDENT          10

#define print(...)           printf("%s", indent); printf(__VA_ARGS__)
#define print_no_indent(...) printf(__VA_ARGS__)

int indent_count = 0;
char indent[PRINT_MAX_INDENT + 1] = "";

#define print_method_start_msg()          print("*********** dma-imx info: %s() start ************\n",__FUNCTION__);\
                                          indent_inc();
#define print_method_end_msg()            indent_dec();\
                                          printf("*********** dma-imx info: %s() end **************\n",__FUNCTION__);

/* ***************************************************************************** */
static inline void indent_inc()
/* ***************************************************************************** */
{
    if (indent_count < PRINT_MAX_INDENT) {
        indent[indent_count] = ' ';
        indent[++indent_count] = 0;
    }
}

/* ***************************************************************************** */
static inline void indent_dec()
/* ***************************************************************************** */
{
    if (indent_count > 0) {
        indent[--indent_count] = 0;
    }
}

/* ***************************************************************************** */
static void print_dma_memory(dma_addr_t *dma_addr_ptr)
/* ***************************************************************************** */
{
    volatile char *ptr  = dma_addr_ptr->vaddr;
    unsigned       size = dma_addr_ptr->len;
    int i;
    char c;
    char txt[2 * PRINT_DMA_MEMORY_MAX_COL * 2];

    if (ptr == NULL) {
        return;
    }
    print_no_indent("address: 0x%p (0x%p), size: %d\n", dma_addr_ptr->vaddr, (void *)(size_t)dma_addr_ptr->paddr, size);
    while (size > 0) {
        print("0x%p: ", ptr);
        memset(txt, 0, sizeof(txt));
        for (i = 0; i < PRINT_DMA_MEMORY_MAX_COL; i++) {
            c = *(ptr++);
            print_no_indent("%02x ", c);
            if (isprint(c) == 0) {
                txt[i] = '.';
            } else {
                txt[i] = c;
            }
            if (--size == 0) {
                while (++i < PRINT_DMA_MEMORY_MAX_COL) {
                    print_no_indent("   ");
                }
                break;
            }
        }
        print(" %s\n", txt);
    }
}

/* ***************************************************************************** */
static void print_dma_transfer(const dma_transfer_t *dma_tr_ptr)
/* ***************************************************************************** */
{
    int i;

    print("DMA transfer memory dump 0x%p: \n", dma_tr_ptr);
    if (dma_tr_ptr != NULL) {
        indent_inc();
        print("src_flags: 0x%08x, dst_flags: 0x%08x, mode_flags: 0x%08x\n", dma_tr_ptr->src_flags, dma_tr_ptr->dst_flags,
              dma_tr_ptr->mode_flags);
        print("xfer_unit_size: %d, xfer_bytes: %d\n", dma_tr_ptr->xfer_unit_size, dma_tr_ptr->xfer_bytes);
        print("src_fragments: %d, dst_fragments: %d\n", dma_tr_ptr->src_fragments, dma_tr_ptr->dst_fragments);
        indent_inc();
        for (i = 0; i < dma_tr_ptr->src_fragments; i++) {
            print("src_fragments[%d], ", i);
            indent_inc();
            print_dma_memory(&dma_tr_ptr->src_addrs[i]);
            indent_dec();
            if (dma_tr_ptr->src_flags & DMA_ADDR_FLAG_SEGMENTED) {
                break;
            }
        }
        for (i = 0; i < dma_tr_ptr->dst_fragments; i++) {
            print("dst_fragments[%d], ", i);
            indent_inc();
            print_dma_memory(&dma_tr_ptr->dst_addrs[i]);
            indent_dec();
            if (dma_tr_ptr->dst_flags & DMA_ADDR_FLAG_SEGMENTED) {
                break;
            }
        }
        indent_dec();
        indent_dec();
    }
}

/* ***************************************************************************** */
static void print_TCD(IMX_eDMA_TCD_t *pTCD)
/* ***************************************************************************** */
{
    off64_t phy_addr;

    mem_offset64(pTCD, NOFD, 1, &phy_addr, 0);

    print("TCD: 0x%p (0x%p)\n", pTCD, (void *)(size_t)phy_addr);
    if (pTCD != NULL) {
        indent_inc();
        print("SADDR    0x%x\n", pTCD->DMA_TCDn_SADDR.R);
        print("SOFF     0x%x\n", pTCD->DMA_TCDn_SOFF.R);
        print("DADDR    0x%x\n", pTCD->DMA_TCDn_DADDR.R);
        print("DOFF     0x%x\n", pTCD->DMA_TCDn_DOFF.R);
        print("ATTR     0x%x\n", pTCD->DMA_TCDn_ATTR.R);
        print("NBYTE    0x%x\n", pTCD->DMA_TCDn_NBYTES_MLOFFNO.R);
        print("SLAST_SD 0x%x\n", pTCD->DMA_TCDn_SLAST_SDA.R);
        print("DLAST_SG 0x%x\n", pTCD->DMA_TCDn_DLAST_SGA.R);
        print("CITER    0x%x\n", pTCD->DMA_TCDn_CITER_ELINKNO.R);
        print("BITER    0x%x\n", pTCD->DMA_TCDn_BITER_ELINKNO.R);
        print("CSR      0x%x", pTCD->DMA_TCDn_CSR.R);
        print_no_indent("(BWC: %d, MAJORLINKCH: %d, ESDA: %d, EEOP: %d, MAJORELINK: %d, ESG: %d, DREQ: %d, INTHALF: %d, INTMAJOR: %d, START: %d)\n",
                        pTCD->DMA_TCDn_CSR.B.BWC,
                        pTCD->DMA_TCDn_CSR.B.MAJORLINKCH,
                        pTCD->DMA_TCDn_CSR.B.ESDA,
                        pTCD->DMA_TCDn_CSR.B.EEOP,
                        pTCD->DMA_TCDn_CSR.B.MAJORELINK,
                        pTCD->DMA_TCDn_CSR.B.ESG,
                        pTCD->DMA_TCDn_CSR.B.DREQ,
                        pTCD->DMA_TCDn_CSR.B.INTHALF,
                        pTCD->DMA_TCDn_CSR.B.INTMAJOR,
                        pTCD->DMA_TCDn_CSR.B.START
                       );
        indent_dec();
    }
}

/* ***************************************************************************** */
static void print_channel_regs(IMX_eDMA_Channel_t *chn_regs_ptr)
/* ***************************************************************************** */
{
    print("chn_regs_ptr: 0x%p\n", chn_regs_ptr);
    if (chn_regs_ptr != NULL) {
        indent_inc();
        print("CH_CSR   0x%x\n", chn_regs_ptr->DMA_CHN_CSR.R);
        print("CH_ES    0x%x\n", chn_regs_ptr->DMA_CHN_ES.R);
        print("CH_INT   0x%x\n", chn_regs_ptr->DMA_CHN_INT.R);
        print("CH_SBR   0x%x\n", chn_regs_ptr->DMA_CHN_SBR.R);
        print("CH_PRI   0x%x\n", chn_regs_ptr->DMA_CHN_PRI.R);
        print_TCD(&chn_regs_ptr->TCD);
        indent_dec();
    }
}
/* ***************************************************************************** */
static void print_chn_info(chn_info_t* chn_info_ptr)
/* ***************************************************************************** */
{
    print("chn_info_ptr: 0x%p\n", chn_info_ptr);
    if (chn_info_ptr != NULL) {
        indent_inc();
        print("chn_info_ptr->chn_regs_ptr:  0x%p\n",   chn_info_ptr->chn_regs_ptr);
        print("chn_info_ptr->chn_irq:       %d\n",     chn_info_ptr->chn_irq);
        print("chn_info_ptr->chn_int_id     %d\n",     chn_info_ptr->chn_int_id);
        print("chn_info_ptr->error_irq:     %d\n",     chn_info_ptr->error_irq);
        print("chn_info_ptr->error_int_id:  %d\n",     chn_info_ptr->error_int_id);
        print("chn_info_ptr->attach_flags:  %d\n",     chn_info_ptr->attach_flags);
        print("chn_info_ptr->chn_number:    %d\n",     chn_info_ptr->chn_number);
        print("chn_info_ptr->tcd_buffer:    0x%p\n",   chn_info_ptr->tcd_buffer);
        print("chn_info_ptr->tcd_array_phy: 0x%p\n",   chn_info_ptr->tcd_array_phy);
        print("chn_info_ptr->error_status:  0x%08x\n", chn_info_ptr->error_status);
        indent_dec();
    }
}
#endif

/**
 * Converts read/write data bus transfer size from number of bits to the value
 * required by SSIZE/DSIZE bit-field in the TCDa_ATTR register.
 *
 * @param num_of_bits r/w data bus transfer size, e.g. 8, 16, 32, ....
 *
 * @return SSIZE/DSIZE value or -1 in case of unsupported value.
 */
static inline uint32_t get_rw_size(const uint32_t num_of_bits)
{
    switch (num_of_bits) {
        case IMX_DMA_NBITS_8:
            return 0;
        case IMX_DMA_NBITS_16:
            return 1;
        case IMX_DMA_NBITS_32:
            return 2;
        case IMX_DMA_NBITS_64:
            return 3;
        case IMX_DMA_NBITS_128:
            return 4;
        case IMX_DMA_NBITS_256:
            return 5;
        case IMX_DMA_NBITS_512:
            return 6;
        default:
            return (uint32_t) - 1; /* Unsupported value */
    }
}

/**
 * eDMA channel interrupt request handler.
 *
 * @param chn_info_ptr channel descriptor pionter.
 *
 */
static void channel_irq_handler(chn_info_t * const chn_info_ptr)
{
    if (chn_info_ptr) {
        struct sigevent *const event = chn_info_ptr->event;
        if (chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR) {
            chn_info_ptr->error_status |= chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.R;  /* Remember error status */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR  = 1;                       /* Clear error interrupt request */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.ERQ = 0;                       /* Disable HW request */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.EEI = 0;                       /* Disable error interrupt */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_INT.B.INT = 1;                       /* Clear channel interrupt request */
            if (event != NULL) {
                MsgSendPulse(event->sigev_coid, event->sigev_priority, event->sigev_code, -1);
            }
        }
        if (chn_info_ptr->chn_regs_ptr->DMA_CHN_INT.B.INT) {
            chn_info_ptr->chn_regs_ptr->DMA_CHN_INT.B.INT = 1;    /* Clear channel interrupt request */
            /* Send info about sent bytes */
            if (chn_info_ptr->sig_bytes_sent) {
                if (event != NULL) {
                    /* Calculate number of transfered bytes from the address stored in SDA buffer and origin stored in TCD buffer. */
                    event->sigev_value.sival_int = (int)(chn_info_ptr->tcd_last_sda[chn_info_ptr->tcd_last_sda_idx] - chn_info_ptr->tcd_buffer[chn_info_ptr->tcd_last_sda_idx].DMA_TCDn_DADDR.R);
                }
                /* Increment buffer indexer */
                if ((++chn_info_ptr->tcd_last_sda_idx) == chn_info_ptr->tcd_cnt) {
                    chn_info_ptr->tcd_last_sda_idx = 0;
                }
            }
            if (event != NULL) {
                MsgSendPulse(event->sigev_coid, event->sigev_priority, event->sigev_code, 0);
            }
        }
    }
}

/**
 * eDMA error interrupt request handler.
 *
 * @param chn_info_ptr channel descriptor pionter.
 *
 */
static void error_irq_handler(chn_info_t * const chn_info_ptr)
{
    if (chn_info_ptr) {
        if (chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR) {
            chn_info_ptr->error_status |= chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.R;  /* Remember error status */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR  = 1;                       /* Clear error interrupt request */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.ERQ = 0;                       /* Disable HW request */
            chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.EEI = 0;                       /* Disable error interrupt */
            const struct sigevent *const event = chn_info_ptr->event;
            if (event != NULL) {
                MsgSendPulse(event->sigev_coid, event->sigev_priority, event->sigev_code, -1);
            }
        }
    }
}

#define DMA_THREAD_INIT     1
#define DMA_THREAD_ERROR    2
#define DMA_THREAD_RUNNING  3

#define DMA_CHN_EVENT       1
#define DMA_ERR_EVENT       2
#define DMA_SHUTDOWN_EVENT  3

/**
 * eDMA error interrupt request handler.
 *
 * @param arg channel descriptor pionter.
 *
 * @return NULL when fails.
 */
static void *dma_irq_thread(void* const arg)
{
    chn_info_t *chn_info_ptr = arg;
    char thread_name[_NTO_THREAD_NAME_MAX];
    int  chid, coid;
    int status = EOK;

    snprintf(thread_name, sizeof(thread_name), "IMX8 DMA IRQ thread");
    pthread_setname_np(0, thread_name);

    chid = ChannelCreate(_NTO_CHF_PRIVATE);
    if (chid == -1) {
        status = errno;
        fprintf(stderr, "%s: ChannelCreate error %d(%s)\n", __func__, errno, strerror(errno));
        goto fail0;
    }

    coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        status = errno;
        fprintf(stderr, "%s: ConnectAttach error %d(%s)\n", __func__, errno, strerror(errno));
        goto fail1;
    }
    chn_info_ptr->tcoid = coid;

    struct sigevent dmaevent;
    SIGEV_PULSE_INIT(&dmaevent, coid, (short)chn_info_ptr->tprio, DMA_ERR_EVENT, NULL);

    chn_info_ptr->error_int_id = InterruptAttachEvent(chn_info_ptr->error_irq, &dmaevent, _NTO_INTR_FLAGS_TRK_MSK);
    if (chn_info_ptr->error_int_id == -1) {
        status = errno;
        fprintf(stderr, "%s: error IRQ %d InterruptAttachEvent error %d(%s)\n",
                            __func__, chn_info_ptr->error_irq, errno, strerror(errno));
        goto fail2;
    }

    SIGEV_PULSE_INIT(&dmaevent, coid, (short)chn_info_ptr->tprio, DMA_CHN_EVENT, NULL);
    chn_info_ptr->chn_int_id = InterruptAttachEvent(chn_info_ptr->chn_irq, &dmaevent, _NTO_INTR_FLAGS_TRK_MSK);
    if (chn_info_ptr->chn_int_id == -1) {
        status = errno;
        fprintf(stderr, "%s: channel IRQ %d InterruptAttachEvent error %d(%s)\n",
                            __func__, chn_info_ptr->chn_irq, errno, strerror(errno));
        goto fail3;
    }

    pthread_sleepon_lock();
    chn_info_ptr->tstate = DMA_THREAD_RUNNING;
    pthread_sleepon_signal(&chn_info_ptr->tstate);
    pthread_sleepon_unlock();

    struct _pulse pulse;
    for (;;) {
        if (MsgReceivePulse(chid, &pulse, sizeof(pulse), NULL) == -1) {
            status = errno;
            fprintf(stderr, "%s: MsgReceivePulse error %d(%s)\n", __func__, errno, strerror(errno));
            break;
        }
        if (pulse.code == DMA_CHN_EVENT) {
            channel_irq_handler(chn_info_ptr);
            InterruptUnmask(chn_info_ptr->chn_irq, chn_info_ptr->chn_int_id);
        } else if (pulse.code == DMA_ERR_EVENT) {
            error_irq_handler(chn_info_ptr);
            InterruptUnmask(chn_info_ptr->error_irq, chn_info_ptr->error_int_id);
        } else if (pulse.code == DMA_SHUTDOWN_EVENT) {
            chn_info_ptr->tstate = 0;
            break;
        } else {
            fprintf(stderr, "%s: MsgReceivePulse unexpected pulse code %d\n", __func__, pulse.code);
            continue;
        }
    }

    InterruptDetach(chn_info_ptr->chn_irq);
fail3:
    InterruptDetach(chn_info_ptr->error_irq);
fail2:
    ConnectDetach(coid);
fail1:
    ChannelDestroy(chid);
fail0:
    if (chn_info_ptr->tstate == DMA_THREAD_INIT) {
        /* Init error, signal parent thread */
        pthread_sleepon_lock();
        chn_info_ptr->tstate = DMA_THREAD_ERROR;
        pthread_sleepon_signal(&chn_info_ptr->tstate);
        pthread_sleepon_unlock();
        errno = status;
    } else if (chn_info_ptr->tstate == DMA_THREAD_RUNNING) {
        /* fatal error, eixt */
        errno = status;
        exit(EXIT_FAILURE);
    } else {
        ; /*termination */
    }
    return NULL;
}

/**
 * dma irq init
 *
 * @param chn_info_ptr channel descriptor pointer
 *
 * @return 0 on success or -1 when fails.
 */
static int dmairq_init(chn_info_t * const chn_info_ptr)
{
    struct sched_param  param;
    pthread_t           tid;
    pthread_attr_t      pattr; /* pthread attr for DMA irq thread */
    int                 ret;

    if (chn_info_ptr == NULL) {
        errno = EINVAL;
        return (-1);
    }

    pthread_attr_init(&pattr);
    pthread_attr_setschedpolicy(&pattr, SCHED_RR);
    param.sched_priority = chn_info_ptr->tprio;
    pthread_attr_setschedparam(&pattr, &param);
    pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

    chn_info_ptr->tstate = DMA_THREAD_INIT;

    ret = pthread_create(&tid, &pattr, dma_irq_thread, chn_info_ptr);
    pthread_attr_destroy(&pattr);
    if (ret != EOK) {
        fprintf(stderr, "%s: pthread_create failed (%s)\n",
                                __FUNCTION__, strerror(ret));
        errno = ret;
        return (-1);
    }

    pthread_sleepon_lock();
    while (chn_info_ptr->tstate == DMA_THREAD_INIT) {
        pthread_sleepon_wait(&chn_info_ptr->tstate);
    }
    pthread_sleepon_unlock();

    if (chn_info_ptr->tstate != DMA_THREAD_RUNNING) {
        errno = EIO;
        return (-1);
    }

    chn_info_ptr->tid = tid;

    return 0;
}

/**
 * dma irq de-init
 *
 * @param chn_info_ptr channel descriptor pointer
 *
 */
static void dmairq_fini(chn_info_t * const chn_info_ptr)
{
    if (chn_info_ptr->tid != -1) {
        /* signal DMA thread to terminate */
        MsgSendPulse(chn_info_ptr->tcoid, -1, DMA_SHUTDOWN_EVENT, 0);
        pthread_join(chn_info_ptr->tid, NULL);
        chn_info_ptr->tstate = 0;
        chn_info_ptr->tid = -1;
        chn_info_ptr->tcoid = -1;
    }
    if (chn_info_ptr->error_irq != -1) {
        InterruptDetach(chn_info_ptr->error_irq);
        chn_info_ptr->error_irq = -1;
    }
    if (chn_info_ptr->chn_irq != -1) {
        InterruptDetach(chn_info_ptr->chn_irq);
        chn_info_ptr->chn_irq = -1;
    }
}

/**
 * Deallocate channel resources.
 *
 * @param chan_ptr Pointer to the SDMA library Channel control structure.
 */
static void chan_free_resources(chn_info_t *chn_info_ptr)
{
    int status;
    rsrc_request_t   req = { 0 };

    dmairq_fini(chn_info_ptr);

    if (chn_info_ptr->chn_regs_ptr != NULL) {
        print("Calling munmap_device_memory(address: %p)\n", chn_info_ptr->chn_regs_ptr);
        munmap_device_memory(chn_info_ptr->chn_regs_ptr, IMX_DMA_CHANNEL_SIZE);
    }

    if (chn_info_ptr->tcd_buffer != NULL) {
        print("Calling munmap_device_memory(address: %p)\n", chn_info_ptr->chn_regs_ptr);
        munmap(chn_info_ptr->tcd_buffer, sizeof(IMX_eDMA_TCD_t) * MAX_DESCRIPTORS);
    }
    if (chn_info_ptr->tcd_last_sda != NULL) {
        print("Calling munmap_device_memory(address: %p)\n", chn_info_ptr->chn_regs_ptr);
        munmap(chn_info_ptr->tcd_last_sda, sizeof(uint32_t) * MAX_DESCRIPTORS);
    }

    if (chn_info_ptr->fd_posix != NOFD) {
        close(chn_info_ptr->fd_posix);
    }

    status = pthread_mutex_destroy(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: pthread_mutex_destroy failed with status %u\n", __FUNCTION__, status);
    }

    if (chn_info_ptr->sop) {
        struct smmu_map_entry entry[2] = {0};
        entry[0].phys = (uintptr64_t)chn_info_ptr->tcd_array_phy;
        entry[0].len  = sizeof(IMX_eDMA_TCD_t) * MAX_DESCRIPTORS;
        entry[1].phys = (uintptr64_t)chn_info_ptr->tcd_last_sda_phy;
        entry[1].len  = sizeof(uint32_t) * MAX_DESCRIPTORS;
        smmu_mapping_add (chn_info_ptr->sop, SMF_NONE, 0, 2, entry, 0);
        smmu_device_add_mmio(NULL, chn_info_ptr->chn_base, IMX_DMA_CHANNEL_SIZE );
    }

    if ((int32_t)chn_info_ptr->chn_number > -1) {
        req.length = 1;
        req.start  = imx_edma_request_get_idx(chn_info_ptr->request_source) - chn_info_ptr->resdb_offset;
        req.end    = req.start;
        req.flags  = RSRCDBMGR_DMA_CHANNEL | RSRCDBMGR_FLAG_RANGE;
        print("Calling rsrcdbmgr_detach(dma channel: %d)\n", chn_info_ptr->chn_number);
        if (rsrcdbmgr_detach(&req, 1) != EOK) {
            fprintf(stderr, "%s: rsrcdbmgr_detach failed with status %s\n", __FUNCTION__, strerror(errno));
        }
    }

    if (chn_info_ptr->event != NULL) {
        free(chn_info_ptr->event);
    }
    free(chn_info_ptr);
}

/**
 * Configures SBR attributes
 *
 * The ATTR register outputs the registers values onto the AHB system bus interface for further
 * decode by the security system. It is also used to support a short-cut routing of peripheral
 * I/O accesses via a local interconnect (bypassing the SMMU).
 *
 * @param chn_info_ptr Channel handler.
 * @param src_phy      Source physical address.
 * @param dst_phy      Destination physical address.
 * @param src_flags    Source flags.
 * @param dst_flags    Destination flags.
 */
static void imx_edma_set_bus_attr(chn_info_t *chn_info_ptr, const uint32_t src_phy, const uint32_t dst_phy,
                           const dma_xfer_flags src_flags, const dma_xfer_flags dst_flags)
{
    uint32_t rd = IMX_DMA_SBR_ATTR_RD;
    uint32_t wr = IMX_DMA_SBR_ATTR_WR;
#ifdef IMX_EDMA_ERRATA_SBR_ATTR_SWAP
    /* Check ERRATA on RD/WR swap */
    if (chn_info_ptr->errata & IMX_EDMA_ERRATA_SBR_ATTR_SWAP) {
        rd = IMX_DMA_SBR_ATTR_WR;
        wr = IMX_DMA_SBR_ATTR_RD;
    }
#endif
    /* Check if source address is in same subsystem and DMA_ADDR_FLAG_DEVICE flag is set */
    if (((src_phy & IMX_SUBSYSTEM_MASK) == (chn_info_ptr->chn_base & IMX_SUBSYSTEM_MASK))
        && (src_flags & DMA_ADDR_FLAG_DEVICE)) {
        chn_info_ptr->chn_regs_ptr->DMA_CHN_SBR.R |=  rd;
    } else {
        /* Disable local interconnect. Use external bus */
        chn_info_ptr->chn_regs_ptr->DMA_CHN_SBR.R &= ~rd;
    }
    /* Check if destination address is in same subsystem and DMA_ADDR_FLAG_DEVICE flag is set */
    if (((dst_phy & IMX_SUBSYSTEM_MASK) == (chn_info_ptr->chn_base & IMX_SUBSYSTEM_MASK))
        && (dst_flags & DMA_ADDR_FLAG_DEVICE)) {
        chn_info_ptr->chn_regs_ptr->DMA_CHN_SBR.R |=  wr;
    } else {
        /* Disable local interconnect. Use external bus */
        chn_info_ptr->chn_regs_ptr->DMA_CHN_SBR.R &= ~wr;
    }
}

/**
 * Check 32 bit address boundary
 *
 * @param paddr Physical address to check
 * @param len   length of the memory to check
 *
 * @return 0 on success or -1 when fails.
 */
static int check_address_boundary(const off64_t paddr, uint32_t len)
{
    off64_t paddr_to_verify;

    if (len) {
        len -= 1;
    }

    paddr_to_verify = paddr + (off64_t)len;
    if (paddr_to_verify > 0xFFFFFFFF) {
        fprintf(stderr, "Physical address is not below 4 GB. 0x%lx\n", paddr_to_verify);
        return -1;
    }
    return 0;
}
/******************************************************************************/
/*                                   API                                      */
/******************************************************************************/

/**
 * Fill dma_driver_info_t structure.
 *
 * @param info Driver information structure address.
 *
 * @return Always returns 0.
 */
static int imx_edma_driver_info(dma_driver_info_t *info)
{
    info->dma_version_major = DMALIB_VERSION_MAJOR;
    info->dma_version_minor = DMALIB_VERSION_MINOR;
    info->dma_rev           = DMALIB_REVISION;
    info->lib_rev           = 0;
    info->description       = IMX_DMA_DESCRIPTION_STR;
    info->num_channels      = edma_chn_hi + 1;
    info->max_priority      = IMX_DMA_CH_PRIO_HI;
    return 0;
}

/**
 * Fill dma_channel_info_t structure.
 *
 * @param channel Channel number.
 * @param info    Channel information structure address.
 *
 * @return Always returns 0.
 */
static int imx_edma_channel_info(__attribute__((unused)) const unsigned channel, dma_channel_info_t *info)
{
    info->max_xfer_size        = 0xFFFC;
    info->max_src_fragments    = MAX_DESCRIPTORS;
    info->max_dst_fragments    = MAX_DESCRIPTORS;
    info->max_src_segments     = 0;
    info->max_dst_segments     = 0;
    /* Supports 1, 2, 4 byte unit sizes */
    info->xfer_unit_sizes      = 0x4 | 0x2 | 0x1;
    info->caps                 = DMA_CAP_EVENT_ON_COMPLETE |
                                 DMA_CAP_EVENT_PER_SEGMENT |
                                 DMA_CAP_DEVICE_TO_MEMORY |
                                 DMA_CAP_MEMORY_TO_DEVICE |
                                 DMA_CAP_MEMORY_TO_MEMORY;
    info->mem_lower_limit      = 0x00000000;
    info->mem_upper_limit      = 0xFFFFFFFF;
    info->mem_nocross_boundary = 0;
    return 0;
}

/**
 * Parse channel options from passed string
 *
 * @param chn_info_ptr Channel handler.
 * @param options      Pointer to a string to parse options.
 *                     Supported options :
 *                     tcd_reload - values 0, 1.Reloads TCD register from TCD buffer when calling xfer_start().
 *                     bw_ctrl - Bus bandwidth control, stalls 0, 4, 8 cycles after each R/W. Default is 0 cycles.
 *                     eeop - Enable end-of-packet processing.
 *                     ecp - Channel can be temporarily suspended by the service request of a higher priority channel.
 *                     dpa - Channel cannot suspend any channel, regardless of channel priority.
 *
 * @return             Execution status.
 */
static int parse_channel_options(chn_info_t * chn_info_ptr, const char *const options)
{
    char    *value;
    int     opt;
    char    *temp_ptr = NULL;
    char    *temp_ptr_head = NULL;

    char * channel_create_opts[] = {
        "tcd_reload",           /* Reloads TCD register from TCD buffer when calling xfer_start(). */
        "bw_ctrl",              /* Bus bandwidth control, stalls 0, 4, 8 cycles after each R/W. Default is 0 cycles. */
        "eeop",                 /* Enable end-of-packet processing */
        "ecp",                  /* Channel can be temporarily suspended by the service request of a higher priority
                                   channel. Disabled by default. */
        "dpa",                  /* Channel cannot suspend any channel, regardless of channel priority. Disabled by default. */
        "sig_bytes_sent",       /* Signal bytes sent via a sigevent from interrupt handler. Usable only if destination address is incremented by eDMA. eg. UART RX. */
        "tprio",                /* Channel DMA IRQ thread priority, 21 by default. */
        NULL
    };

    chn_info_ptr->tcd_reload = 0;
    chn_info_ptr->bwc = 0;
    chn_info_ptr->eeop = 0;
    chn_info_ptr->dpa = 0;
    chn_info_ptr->ecp = 0;
    chn_info_ptr->sig_bytes_sent = 0;
    chn_info_ptr->tprio = 21;

    if (options) {
        temp_ptr_head = strdup(options);
        temp_ptr = temp_ptr_head;
    }

    while (temp_ptr && (*temp_ptr != '\0')) {
        opt = getsubopt(&temp_ptr, channel_create_opts, &value);
        if (opt == -1) {
            free(temp_ptr_head);
            return -1;
        }

        switch (opt) {
            case 0:
                chn_info_ptr->tcd_reload = (unsigned int)strtoul(value, NULL, 0);
                if (chn_info_ptr->tcd_reload > 1) {
                    chn_info_ptr-> tcd_reload = 1;
                }
                break;
            case 1:
                chn_info_ptr->bwc = (unsigned int)strtoul(value, NULL, 0);
                switch (chn_info_ptr->bwc) {
                    case 4:
                        /* Register value is 2 */
                        chn_info_ptr->bwc = 0x2;
                        break;
                    case 8:
                        /* Register value is 3 */
                        chn_info_ptr->bwc = 0x3;
                        break;
                    default:
                        /* Otherwise disable */
                        chn_info_ptr->bwc = 0x0;
                        break;
                }
                break;
            case 2:
                chn_info_ptr->eeop = (unsigned int)strtoul(value, NULL, 0);
                if (chn_info_ptr->eeop > 1) {
                    chn_info_ptr->eeop = 1;
                }
                break;
            case 3:
                chn_info_ptr->ecp = (uint8_t )strtoul(value, NULL, 0);
                if (chn_info_ptr->ecp > 1) {
                    chn_info_ptr->ecp = 1;
                }
                break;
            case 4:
                chn_info_ptr->dpa = (uint8_t)strtoul(value, NULL, 0);
                if (chn_info_ptr->dpa > 1) {
                    chn_info_ptr->dpa = 1;
                }
                break;
            case 5:
                chn_info_ptr->sig_bytes_sent = (uint32_t)(strtoul(value, NULL, 0) ? 1 : 0);
                break;
            case 6:
                chn_info_ptr->tprio = (int)strtoul(value, NULL, 0);
                break;
            default:
                free(temp_ptr_head);
                return -1;
        }
    }
    if (temp_ptr_head) {
        free(temp_ptr_head);
    }
    return 0;
}

static void imx_edma_query_channel (void *const handle, dma_channel_query_t *chninfo)
{
    const chn_info_t *const chn  = handle;

    chninfo->chan_idx = chn->chn_number;
    chninfo->irq = (uint32_t)chn->chn_irq;
    /* Store the error code in reserved[0] */
    chninfo->reserved[0] = chn->error_status;
}

/**
 * Prepare channel for data transfer.
 *
 * @param optstring Optional string with channel options.
 *                  Supported options :
 *                  tcd_reload - values 0, 1.Reloads TCD register from TCD buffer when calling xfer_start().
 *                  bw_ctrl    - Bus bandwidth control, stalls 0, 4, 8 cycles after each R/W. Default is 0 cycles.
 *                  eeop       - values 0, 1. Enable end-of-packet processing.
 *                  ecp        - values 0, 1. Channel can be temporarily suspended by the service request of a higher priority
 *                               channel. Disabled by default.
 *                  dpa        - values 0, 1. Channel cannot suspend any channel, regardless of channel priority.
 *                               Disabled by default.
 *                  sig_bytes_sent - signal bytes sent via a sigevent from interrupt handler.
 *                                   Usable only if destination address is incremented by eDMA.
 *
 * @param event     Event signaled on interrupt.
 * @param channel   Channel type variable address  (Channel type value: e.g. IMX_DMA_REQ_SW or hardware DMA request).
 * @param prio      Channel priority in range [IMX_edma_CH_PRIO_LO(1)..IMX_edma_CH_PRIO_HI(7)].
 * @param flags     Channel attache flags (DMA_ATTACH_EVENT_ON_COMPLETE, DMA_ATTACH_EVENT_PER_SEGMENT, DMA_ATTACH_ANY_CHANNEL are supported by this driver).
 * @return          channnel handle on success or NULL on error(errno is set).
 */
static void *imx_edma_channel_attach(const char *const optstring, const struct sigevent *const event, unsigned *const channel, const int prio,
                                     const  dma_attach_flags flags)
{
    chn_info_t         *chn_info_ptr = NULL;
    rsrc_request_t      req;
    int                 status = EOK;
    const edma_device_info_t *info;
    unsigned            i, chnl;

    print_method_start_msg();
    do {
        if (!edma_device_info) {
            fprintf(stderr, "%s: edma_device_info not allocated by ctor\n", __FUNCTION__);
            status = EINVAL;
            break;
        }
        /* Allocate zero initialized memory for channel descriptor */
        chn_info_ptr = calloc(1, sizeof(chn_info_t));
        if (chn_info_ptr == NULL) {
            fprintf(stderr, "%s: calloc(size: %zx) failed: %s\n", __FUNCTION__, sizeof(chn_info_t), strerror(errno));
            break;
        }
        /* Initialize to -1 so we can tell if we have done the rsrcdbmgr_attach() */
        chn_info_ptr->chn_number = (uint32_t) -1;

        status = pthread_mutex_init(&chn_info_ptr->lock, NULL);
        if (status != EOK) {
            status = errno;
            fprintf(stderr, "%s: pthread_mutex_init failed with status %u\n", __FUNCTION__, status);
            break;
        }

        /* Parse channel options */
        if (parse_channel_options(chn_info_ptr, optstring) != 0) {
            fprintf(stderr, "%s: failed to parse channel options\n", __FUNCTION__);
            /* This is not critical issue, so continue in channel initialization */
        }

        if (flags & DMA_ATTACH_ANY_CHANNEL) {
            /* SW request channel allocation */
            chn_info_ptr->request_source = IMX_DMA_REQ_SW;
        } else if (channel != NULL) {
            /* HW request channel allocation */
            chn_info_ptr->request_source = *channel;
            chn_info_ptr->resdb_offset = edma_device_info[imx_edma_request_get_dma_number(chn_info_ptr->request_source)].resdb_offset;
        } else {
            /* Wrong input. DMA_ATTACH_ANY_CHANNEL or channel number has to be set. */
            status = EINVAL;
            fprintf(stderr, "%s: Wrong input parameters. Specify channel or use DMA_ATTACH_ANY_CHANNEL flag.\n", __FUNCTION__);
            break;
        }
        /* Get channel from resource database manager. */
        memset(&req, 0, sizeof(rsrc_request_t));
        if (flags & DMA_ATTACH_ANY_CHANNEL) {
            /* Get any channel from database */
            req.start  = edma_chn_lo;
            req.end = edma_chn_hi;
        } else {
            /* Get specific channel from database */
            req.start = imx_edma_request_get_idx(chn_info_ptr->request_source) - chn_info_ptr->resdb_offset;
            req.end = req.start;
        }
        req.length = 1;
        req.flags  = RSRCDBMGR_DMA_CHANNEL | RSRCDBMGR_FLAG_RANGE;
        if (rsrcdbmgr_attach(&req, 1) == -1) {
            status = errno;
            fprintf(stderr, "%s: rsrcdbmgr_attach(request: 0x%jx) failed: %s\n", __FUNCTION__, req.start,  strerror(errno));
            break;
        }
        /* Calling thread is now exclusive owner of the channel */
        if (flags & DMA_ATTACH_ANY_CHANNEL) {
            print("Got DMA%u channel %u \n", imx_edma_request_get_dma_number(req.start),
                  imx_edma_request_get_channel_number(req.start));

            chnl = (unsigned)req.start;
            for (i = 0; i < edma_device_count; i++) {
                if (chnl > edma_device_info[i].chn_num) {
                    chnl -= edma_device_info[i].chn_num;
                } else {
                    chn_info_ptr->resdb_offset = edma_device_info[i].resdb_offset;
                    chn_info_ptr->request_source |= imx_edma_define_request(i, chnl, edma_device_info[i].irq);
                    break;
                }
            }
            if (channel != NULL) {
                /* Return allocated channel number */
                *channel = chn_info_ptr->request_source;
            }
        }
        info = &edma_device_info[imx_edma_request_get_dma_number(chn_info_ptr->request_source)];
        chn_info_ptr->chn_number = imx_edma_request_get_channel_number(chn_info_ptr->request_source);
        /* Check channel number */
        if (chn_info_ptr->chn_number >= info->chn_num) {
            status = EINVAL;
            fprintf(stderr, "%s: Selected eDMA has only %u channels but request channel number is %u\n", __FUNCTION__,
                    info->chn_num, chn_info_ptr->chn_number);
            break;
        }
        chn_info_ptr->chn_irq = (int)imx_edma_request_get_irq_number(chn_info_ptr->request_source);
        if (chn_info_ptr->chn_irq == IMX_DMA_COMMON_IRQ) {
            /* Use common DMA IRQ which gathers all channels */
            chn_info_ptr->chn_irq = info->irq;
        }
        chn_info_ptr->error_irq      = info->error_irq;
        chn_info_ptr->errata         = info->errata;
        chn_info_ptr->chn_int_id     = -1;
        chn_info_ptr->error_int_id   = -1;
        chn_info_ptr->chn_base       = (info->chn_base + (chn_info_ptr->chn_number << 16));
        chn_info_ptr->chn_regs_ptr = mmap_device_memory((void *)chn_info_ptr->chn_regs_ptr, IMX_DMA_CHANNEL_SIZE,
                                                        PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, chn_info_ptr->chn_base);
        if (chn_info_ptr->chn_regs_ptr == MAP_FAILED) {
            status = errno;
            fprintf(stderr, "%s: mmap_device_memory(address: 0x%p) failed: %s\n", __FUNCTION__, chn_info_ptr->chn_regs_ptr,
                    strerror(errno));
            break;
        }

        if (event != NULL) {
            chn_info_ptr->event = calloc(1, sizeof(struct sigevent));
            if (chn_info_ptr->event == NULL) {
                status = errno;
                fprintf(stderr, "%s: calloc(size: %zx) failed: %s\n", __FUNCTION__,
                                                    sizeof(struct sigevent), strerror(errno));
                break;
            }
            *chn_info_ptr->event  = *event;
        } else {
            chn_info_ptr->event = NULL;
        }

        chn_info_ptr->attach_flags = flags;

        /* Configure channel priority */
        chn_info_ptr->chn_regs_ptr->DMA_CHN_PRI.B.ALP = (uint32_t)((prio <= IMX_DMA_CH_PRIO_HI) ? prio : IMX_DMA_CH_PRIO_HI);
        chn_info_ptr->chn_regs_ptr->DMA_CHN_PRI.B.DPA = chn_info_ptr->dpa;
        chn_info_ptr->chn_regs_ptr->DMA_CHN_PRI.B.ECP = chn_info_ptr->ecp;

        chn_info_ptr->fd_posix = posix_typed_mem_open("sysram&below4G", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
        if (chn_info_ptr->fd_posix == -1) {
            fprintf(stderr, "%s: Failed to open sysram&below4G typed memory(%s)\n", __FUNCTION__, strerror(errno));
            status = errno;
            break;
        }
        chn_info_ptr->tcd_buffer = mmap(NULL, sizeof(IMX_eDMA_TCD_t) * MAX_DESCRIPTORS, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                            MAP_SHARED, chn_info_ptr->fd_posix, 0);
        if (chn_info_ptr->tcd_buffer == MAP_FAILED) {
            status = errno;
            fprintf(stderr, "%s: mmap(tcd_buffer, size: 0x%zx) failed: %s\n", __FUNCTION__,
                    sizeof(IMX_eDMA_TCD_t) * MAX_DESCRIPTORS, strerror(errno));
            break;
        }
        if (mem_offset64(chn_info_ptr->tcd_buffer, NOFD, 1, (off64_t *)&chn_info_ptr->tcd_array_phy, NULL) != 0) {
            status = errno;
            fprintf(stderr, "%s: mem_offset64(tcd_buffer: %p) failed: %s\n", __FUNCTION__, chn_info_ptr->tcd_buffer,
                    strerror(errno));
            break;
        }
        if (dmairq_init(chn_info_ptr) != 0) {
            status = errno;
            fprintf(stderr, "%s: dmairq_init failed:\n", __FUNCTION__);
            break;
        }

        chn_info_ptr->tcd_last_sda = mmap(NULL, sizeof(uint32_t) * MAX_DESCRIPTORS, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                              MAP_SHARED, chn_info_ptr->fd_posix, 0);
        if (chn_info_ptr->tcd_last_sda == MAP_FAILED) {
            status = errno;
            fprintf(stderr, "%s: mmap(tcd_last_sda, size: 0x%zx) failed: %s\n", __FUNCTION__,
                    sizeof(uint32_t) * MAX_DESCRIPTORS, strerror(errno));
            break;
        }
        if (mem_offset64(chn_info_ptr->tcd_last_sda, NOFD, 1, (off64_t *)&chn_info_ptr->tcd_last_sda_phy, NULL) != 0) {
            status = errno;
            fprintf(stderr, "%s: mem_offset64(tcd_last_sda: %p) failed: %s\n", __FUNCTION__, chn_info_ptr->tcd_last_sda,
                    strerror(errno));
            break;
        }
        print_chn_info(chn_info_ptr);
        print_method_end_msg();
        return (void *)chn_info_ptr;
    } while (0);

    if (chn_info_ptr != NULL) {
        print_chn_info(chn_info_ptr);
        chan_free_resources(chn_info_ptr);
    }
    print_method_end_msg();
    errno = status;
    return NULL;
}

/**
 * Prepare channel for data transfer with smmu support
 *
 * @param optstring Optional string with channel options.
 *                  Supported options :
 *                  tcd_reload - values 0, 1.Reloads TCD register from TCD buffer when calling xfer_start().
 *                  bw_ctrl    - Bus bandwidth control, stalls 0, 4, 8 cycles after each R/W. Default is 0 cycles.
 *                  eeop       - values 0, 1. Enable end-of-packet processing.
 *                  ecp        - values 0, 1. Channel can be temporarily suspended by the service request of a higher priority
 *                               channel. Disabled by default.
 *                  dpa        - values 0, 1. Channel cannot suspend any channel, regardless of channel priority.
 *                               Disabled by default.
 *                  sig_bytes_sent - signal bytes sent via a sigevent from interrupt handler.
 *                                   Usable only if destination address is incremented by eDMA.
 *
 * @param event     Event signaled on interrupt.
 * @param channel   Channel type variable address  (Channel type value: e.g. IMX_DMA_REQ_SW or hardware DMA request).
 * @param prio      Channel priority in range [IMX_edma_CH_PRIO_LO(1)..IMX_edma_CH_PRIO_HI(7)].
 * @param flags     Channel attache flags (DMA_ATTACH_EVENT_ON_COMPLETE, DMA_ATTACH_EVENT_PER_SEGMENT, DMA_ATTACH_ANY_CHANNEL are supported by this driver).
 * @param sop       SMMU object to register device and memory mappings against
 * @return          channnel handle on success or NULL on error(errno is set).
 */
static void *imx_edma_channel_attach_smmu (const char *const optstring, const struct sigevent *const event,
                                           unsigned *const channel, const int prio, const dma_attach_flags flags, struct smmu_object *const sop)
{
    int         status = EOK;
    chn_info_t  *handle = NULL;
    struct smmu_map_entry entry[2] = {0};

    handle = imx_edma_channel_attach(optstring, event, channel, prio, flags);
    if (handle == NULL) return (handle);

    if (sop == NULL) return (handle);

    status = smmu_device_add_mmio(sop, handle->chn_base, IMX_DMA_CHANNEL_SIZE);
    if (status == -1) {
        status = errno;
        chan_free_resources(handle);
        errno = status;
        return (NULL);
    }

    entry[0].phys = (uintptr64_t)handle->tcd_array_phy;
    entry[0].len  = sizeof(IMX_eDMA_TCD_t) * MAX_DESCRIPTORS;
    entry[1].phys = (uintptr64_t)handle->tcd_last_sda_phy;
    entry[1].len  = sizeof(uint32_t) * MAX_DESCRIPTORS;
    if (smmu_mapping_add (sop, SMF_READ|SMF_WRITE, 0, 2, entry, 0) == -1) {
        status = errno;
        chan_free_resources(handle);
        errno = status;
        return (NULL);
    }

    handle->sop = sop;
    return (handle);
}

/**
 * Release resources allocated by edma_channel_attach() methods.
 *
 * @param handle Channel handle.
 */
static void imx_edma_channel_release(void *const handle)
{
    chn_info_t *const chn_info_ptr = handle;
    chan_free_resources(chn_info_ptr);
}

/**
 * Frees DMA transfer buffer.
 * @param handle Channel handle.
 * @param addr DMA buffer description structure.
 */
static void imx_dma_free_buffer(void *const handle, dma_addr_t *addr)
{
    chn_info_t *const chn_info_ptr = handle;

    if (addr->len && addr->vaddr) {
        if (chn_info_ptr && chn_info_ptr->sop) {
            struct smmu_map_entry entry;

            entry.phys = (uintptr64_t)addr->paddr;
            entry.len  = addr->len;
            smmu_mapping_add (chn_info_ptr->sop, SMF_NONE, 0, 1, &entry, 0);
        }
        munmap(addr->vaddr, addr->len);
    }
    addr->vaddr = NULL;
    addr->paddr = 0;
    addr->len   = 0;
}

/**
 * Allocates buffer for DMA transfer.
 * @param handle Channel handle. Optional, used only for allocation in posix typed memory object on 64-bit systems with more than 4 GB RAM.
 * @param addr   DMA buffer description structure address.
 * @param size   Requested buffer size.
 * @param flags  Additional flags: DMA_BUF_FLAG_NOCACHE | DMA_BUF_FLAG_SHARED.
 *
 * @return       0 on success or -1 on error(errno is set).
 */
static int imx_dma_alloc_buffer(void *const handle, dma_addr_t *addr, const unsigned size, const unsigned flags)
{
    chn_info_t *const chn_info_ptr = handle;
    int prot_flags = PROT_READ | PROT_WRITE;

    if ((chn_info_ptr == NULL) || (chn_info_ptr->fd_posix == NOFD)) {
        errno = EINVAL;
        return -1;
    }

    if (flags & DMA_BUF_FLAG_NOCACHE) {
        prot_flags |= PROT_NOCACHE;
    }

    /* Allocate a physically contiguous buffer */
    addr->vaddr = mmap(NULL, size, prot_flags, MAP_SHARED, chn_info_ptr->fd_posix, 0);
    if (addr->vaddr == MAP_FAILED) {
        return -1;
    }

    if (mem_offset64(addr->vaddr, NOFD, 1, &addr->paddr, NULL) != 0) {
        return -1;
    }
    addr->len = size;

    /* If the driver allocs the buffer before attaching to the channel
     * then it will have to add the mapping to the smmu object itself
     */
    if (chn_info_ptr && chn_info_ptr->sop) {
        struct smmu_map_entry entry;

        entry.phys = (uintptr64_t)addr->paddr;
        entry.len  = addr->len;
        if (smmu_mapping_add (chn_info_ptr->sop, SMF_READ|SMF_WRITE, 0, 1, &entry, 0) == -1) {
            const int status = errno;
            imx_dma_free_buffer(handle, addr);
            errno = status;
            return -1;
        }
    }

    return 0;
}

/**
 * Fills DMA transfer descriptor.
 *
 * @param tcd_idx       TCD index.
 * @param tcd_ptr       Pointer to TCD memory.
 * @param chn_info_ptr  Pointer to channel info structure.
 * @param tinfo         Pointer to dma_transfer_t structure.
 * @param src_addr      Source address.
 * @param dst_addr      Destination address.
 * @param buff_len      Source and Destination buffer length.
 * @param sof           TCD signed source address offset.
 * @param dof           TCD signed destination address offset.
 * @param attr          TCD transfer attributes.
 * @param last_TCD      Set for last TCD.
 *
 * @return Returns number of bytes to be transfered.
 */
static uint32_t fill_TCD_n(
    const int             tcd_idx,
    IMX_eDMA_TCD_t       *tcd_ptr,
    const chn_info_t     *const chn_info_ptr,
    const dma_transfer_t *const tinfo,
    const uint32_t        src_addr,
    const uint32_t        dst_addr,
    const uint32_t        buff_len,
    const uint16_t        sof,
    const uint16_t        dof,
    const uint16_t        attr,
    const unsigned        last_TCD)
{
    uint32_t          xfer_size, xfer_count;
    const IMX_eDMA_TCD_t *const tcd_ptr_phy = chn_info_ptr->tcd_array_phy;

    if (tinfo->xfer_bytes > buff_len) {
        /* Number of bytes to be transferred per request > destination buffer[i] size */
        xfer_size  = buff_len;
        xfer_count = 1;
    } else {
        xfer_size = tinfo->xfer_bytes;
        xfer_count = buff_len / xfer_size;
    }
    /* Clear tcd buffer */
    memset(tcd_ptr, 0, sizeof(IMX_eDMA_TCD_t));
    xfer_size &= IMX_DMA_TCDn_NBYTES_MLOFFNO_NBYTES_MASK;
    tcd_ptr->DMA_TCDn_SADDR.R   = src_addr;                /* Set source address */
    tcd_ptr->DMA_TCDn_DADDR.R   = dst_addr;                /* Set destination address */
    tcd_ptr->DMA_TCDn_SOFF.R    = sof;
    tcd_ptr->DMA_TCDn_DOFF.R    = dof;
    tcd_ptr->DMA_TCDn_ATTR.R    = attr;
    tcd_ptr->DMA_TCDn_NBYTES_MLOFFNO.R = xfer_size;        /* Minor loop iteration count = fifo_size */
    tcd_ptr->DMA_TCDn_CITER_ELINKNO.R  = (uint16_t)xfer_count;       /* Major loop iteration count */
    tcd_ptr->DMA_TCDn_BITER_ELINKNO.R  = (uint16_t)xfer_count;       /* Major loop iteration count */
    if (last_TCD) {
        /* Last TCD */
        tcd_ptr->DMA_TCDn_DLAST_SGA.R       = (uint32_t)(size_t)tcd_ptr_phy;
        if (!(tinfo->mode_flags & DMA_MODE_FLAG_REPEAT)) {
            tcd_ptr->DMA_TCDn_CSR.R = IMX_DMA_TCDn_CSR_DREQ_MASK;              /* Disable external request */
        }
        if (chn_info_ptr->attach_flags & DMA_ATTACH_EVENT_ON_COMPLETE) {
            tcd_ptr->DMA_TCDn_CSR.B.INTMAJOR = 1;                           /* Enable transfer complete interrupt if required */
        }
    } else {
        tcd_ptr->DMA_TCDn_DLAST_SGA.R  = (uint32_t)(size_t)(tcd_ptr_phy + tcd_idx + 1);
    }
    if (chn_info_ptr->attach_flags & DMA_ATTACH_EVENT_PER_SEGMENT) {
        tcd_ptr->DMA_TCDn_CSR.B.INTMAJOR = 1;                 /* Enable transfer complete interrupt for each TCD if required */
    }
    tcd_ptr->DMA_TCDn_CSR.B.MAJORLINKCH = (uint16_t)tcd_idx;
    tcd_ptr->DMA_TCDn_CSR.B.ESG = 1;
    /* For SW trigger initialize also START bit to start each TCD except first TCD. First TCD is triggered by xfer_start() */
    if ((chn_info_ptr->request_source & IMX_DMA_REQ_SW) && (tcd_idx > 0)) {
        tcd_ptr->DMA_TCDn_CSR.B.START = 1;
    }
    /* Initialize channel bandwidth control */
    if (chn_info_ptr->bwc) {
        tcd_ptr->DMA_TCDn_CSR.B.BWC = (uint16_t)chn_info_ptr->bwc;
    }
    /* Initialize end-of-packet processing */
    if (chn_info_ptr->eeop) {
        tcd_ptr->DMA_TCDn_CSR.B.EEOP = 1;
    }
    if (chn_info_ptr->tcd_last_sda) {
        /* Enable ESDA to provide an information about destination address when major loop completes */
        tcd_ptr->DMA_TCDn_SLAST_SDA.R = (uint32_t)(size_t)(chn_info_ptr->tcd_last_sda_phy + tcd_idx);
        /* SLAST_SDA contains an address to the memory to store a last dest. address */
        tcd_ptr->DMA_TCDn_CSR.B.ESDA = 1;
    }

    print_TCD(tcd_ptr);
    return (xfer_size * xfer_count); /* Return number of bytes transferred */
}

/**
 * Prepare DMA channel for transfer.
 *
 * @param handle Channel handle.
 * @param tinfo  Transfer information structure address.
 *               tinfo->src(dst)_addrs.paddr - Physical 32 bit address of buffer or register. Bellow 4 GB address space.
 *               tinfo->src(dst)_addrs.len   - Buffer size or fragment size or register size in bytes.
 *               tinfo->xfer_unit_size  - Transfer unit size in bits. Eg. for 16 bit access to register xfer_unit_size=16
 *               tinfo->xfer_size       - Number of bytes transfered per request.
 *               Supported source and destination flags:
 *               DMA_ADDR_FLAG_DEVICE - Enables local bus (subsystem bus) between eDMA channel and device registers.
 *                                      This option is recommended for eDMA transfers triggered by peripheral
 *                                      hardware requests.
 *                                      Flag is restricted for device which generates hardware request for channel
 *                                      represented by handle parameter. It is not possible to use it for different
 *                                      channel. Eg. eDMA channel triggered by HW request from GPT device cannot use
 *                                      local bus to write data to eg. ESAI device.
 *               DMA_ADDR_FLAG_NO_INCREMENT - Address is not incremented.
 *               DMA_ADDR_FLAG_DECREMENT - Address is decremented.
 *               DMA_ADDR_FLAG_SEGMENTED - Divides continuous buffer to segments. Number of segments is defined by
 *               src_fragments or dst_fragments. It is usable in case client application wants event on segment
 *               complete (DMA_ATTACH_EVENT_PER_SEGMENT flag used with channel_attach()).
 *               By default source/destination address is automatically incremented by xfer_unit_size/8 count.
 *               Supported mode flags:
 *               DMA_MODE_FLAG_REPEAT - Infinitely repeats DMA transfer (triggered by hardware request).
 *
 * @return Returns 0 on success or -1 on error(errno is set).
 */
static int imx_edma_setup_xfer(void *const handle, const dma_transfer_t *const tinfo)
{
    chn_info_t       *chn_info_ptr = handle;
    IMX_eDMA_TCD_t   *tcd_ptr;
    int               res = -1;
    unsigned int      idx;
    const unsigned int src_data_width_encoded = get_rw_size(tinfo->xfer_unit_size);
    const unsigned int dst_data_width_encoded = get_rw_size(tinfo->xfer_unit_size);

    const uint16_t    src_data_width = (uint16_t)(tinfo->xfer_unit_size / 8);
    const uint16_t    dst_data_width = (uint16_t)(tinfo->xfer_unit_size / 8);

    uint32_t          buff_ptr;
    uint32_t          buff_off;
    uint32_t          buff_n_ptr;
    uint32_t          buff_n_len;
    uint16_t          attr;
    int16_t           sof, dof;
    off64_t           addr;
    uint32_t          src_phy_mask;
    uint32_t          dst_phy_mask;
    uint32_t          len;

    int               status = EOK;

    print_method_start_msg();
    print_dma_transfer(tinfo);

    switch (tinfo->xfer_unit_size) {
        case IMX_DMA_NBITS_8:
        case IMX_DMA_NBITS_16:
        case IMX_DMA_NBITS_32:
            break;
        default:
            fprintf(stderr, "%s: Incorrect xfer_unit_size %u\n", __FUNCTION__, tinfo->xfer_unit_size);
            errno = EINVAL;
            return -1;
    }

    status = pthread_mutex_lock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex lock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }
    do {
        if (tinfo->src_fragments > MAX_DESCRIPTORS) {
            fprintf(stderr, "%s: ERROR tinfo->src_fragments(%d) > MAX_DESCRIPTORS(%d): %s\n", __FUNCTION__, tinfo->src_fragments,
                    MAX_DESCRIPTORS, strerror(errno));
            status = EINVAL;
            break;
        }
        tcd_ptr = chn_info_ptr->tcd_buffer;

        if (tinfo->src_flags & DMA_ADDR_FLAG_DECREMENT) {
            sof = -(int16_t)src_data_width;
        } else if (tinfo->src_flags & DMA_ADDR_FLAG_NO_INCREMENT) {
            sof = 0;
        } else {
            sof = (int16_t)src_data_width;
        }

        if (tinfo->dst_flags & DMA_ADDR_FLAG_DECREMENT) {
            dof = -(int16_t)dst_data_width;
        } else if (tinfo->dst_flags & DMA_ADDR_FLAG_NO_INCREMENT) {
            dof = 0;
        } else {
            dof = (int16_t)dst_data_width;
        }
        attr = (uint16_t)((src_data_width_encoded << IMX_DMA_TCDn_ATTR_SSIZE_SHIFT) + (dst_data_width_encoded <<
                                                                            IMX_DMA_TCDn_ATTR_DSIZE_SHIFT));
        /* Source PHY for configuration of SBR Attributes */
        src_phy_mask  = (uint32_t)tinfo->src_addrs[0].paddr & IMX_SUBSYSTEM_MASK;
        /* Destination PHY for configuration of SBR Attributes */
        dst_phy_mask  = (uint32_t)tinfo->dst_addrs[0].paddr & IMX_SUBSYSTEM_MASK;
        /* Memory to memory transfer */
        if (tinfo->src_fragments == 1) {
            /* Scatter 1-n */
            addr = tinfo->src_addrs[0].paddr;
            len = (sof > 0) ? tinfo->src_addrs[0].len : 0;
            if (check_address_boundary(addr, len) != 0) {
                status = EINVAL;
                break;
            }
            addr += (off64_t)len;

            buff_ptr = (uint32_t)tinfo->src_addrs[0].paddr;     /* Source address */
            buff_n_len = tinfo->dst_addrs[0].len / tinfo->dst_fragments;
            for (idx = 0; idx < tinfo->dst_fragments; idx++) {
                if (tinfo->dst_flags & DMA_ADDR_FLAG_SEGMENTED) {
                    addr = tinfo->dst_addrs[0].paddr + (off64_t)(idx * buff_n_len);
                    buff_n_ptr = (uint32_t)addr;
                } else {
                    addr = tinfo->dst_addrs[idx].paddr;
                    buff_n_ptr = (uint32_t)addr;
                    buff_n_len = tinfo->dst_addrs[idx].len;
                    if ((buff_n_ptr & IMX_SUBSYSTEM_MASK) != (chn_info_ptr->chn_base & IMX_SUBSYSTEM_MASK)) {
                        /* This TCD differs in PHY. It is out of subsystem so we cannot use internal bus for destination */
                        dst_phy_mask = buff_n_ptr & IMX_SUBSYSTEM_MASK;
                    }
                }
                len = (dof > 0) ? buff_n_len : 0;
                if (check_address_boundary(addr, len) != 0) {
                    status = EINVAL;
                    break;
                }
                addr += (off64_t)len;

                print("Dst addr: 0x%p, size: %d\n", (void *)(size_t)buff_n_ptr, buff_n_len);
                buff_off = fill_TCD_n((int)idx, tcd_ptr, chn_info_ptr, tinfo,
                                      buff_ptr,                           /* Source address */
                                      buff_n_ptr,                         /* Destination address */
                                      buff_n_len,                         /* Buffer size */
                                      (uint16_t)sof, (uint16_t)dof, attr, /* Attributes */
                                      (idx == (tinfo->dst_fragments - 1))); /* True for last TCD */
                if (tinfo->src_flags & DMA_ADDR_FLAG_DECREMENT) {
                    buff_ptr -= buff_off;
                } else if (!(tinfo->src_flags & DMA_ADDR_FLAG_NO_INCREMENT)) {
                    buff_ptr += buff_off;
                } else {
                    ; /* no action required */
                }
                tcd_ptr++;
            }
            chn_info_ptr->tcd_cnt = tinfo->dst_fragments;
        } else if (tinfo->dst_fragments == 1) {
            /* Gather n-1 */
            addr = tinfo->dst_addrs[0].paddr;
            len = (dof > 0) ? tinfo->dst_addrs[0].len : 0;
            if (check_address_boundary(addr, len) != 0) {
                status = EINVAL;
                break;
            }
            addr += (off64_t)len;

            buff_ptr = (uint32_t)tinfo->dst_addrs[0].paddr;     /* Destination address */
            buff_n_len = tinfo->src_addrs[0].len / tinfo->src_fragments;
            for (idx = 0; idx < tinfo->src_fragments; idx++) {
                if (tinfo->src_flags & DMA_ADDR_FLAG_SEGMENTED) {
                    addr = tinfo->src_addrs[0].paddr + (off64_t)(idx * buff_n_len);
                    buff_n_ptr = (uint32_t)addr;
                } else {
                    addr = tinfo->src_addrs[idx].paddr;
                    buff_n_ptr = (uint32_t)addr;
                    buff_n_len = tinfo->src_addrs[idx].len;
                    if ((buff_n_ptr & IMX_SUBSYSTEM_MASK) != (chn_info_ptr->chn_base & IMX_SUBSYSTEM_MASK)) {
                        /* This TCD differs in PHY. It is out of subsystem so we cannot use internal bus for source */
                        src_phy_mask = buff_n_ptr & IMX_SUBSYSTEM_MASK;
                    }
                }
                len = (sof > 0) ? buff_n_len : 0;
                if (check_address_boundary(addr, len) != 0) {
                    status = EINVAL;
                    break;
                }
                addr += (off64_t)len;
                print("Src addr: 0x%p, size: %d\n", (void *)(size_t)buff_n_ptr, buff_n_len);
                buff_off = fill_TCD_n((int)idx, tcd_ptr, chn_info_ptr, tinfo,
                                      buff_n_ptr,                         /* Source address */
                                      buff_ptr,                           /* Destination address */
                                      buff_n_len,                         /* Buffer size */
                                      (uint16_t)sof, (uint16_t)dof, attr, /* Attributes */
                                      (idx == tinfo->src_fragments - 1));   /* True for last TCD */
                if (tinfo->dst_flags & DMA_ADDR_FLAG_DECREMENT) {
                    buff_ptr -= buff_off;
                } else if (!(tinfo->dst_flags & DMA_ADDR_FLAG_NO_INCREMENT)) {
                    buff_ptr += buff_off;
                } else {
                    ; /* no action required */
                }
                tcd_ptr++;
            }
            chn_info_ptr->tcd_cnt = tinfo->src_fragments;
        } else {
            fprintf(stderr,
                    "%s: Parameter error. Only source or only destination buffer can be fragmented, not both! src_fragments = %d, dst_fragments = %d\n",
                    __FUNCTION__, tinfo->src_fragments, tinfo->dst_fragments);
            status = EINVAL;
            break;
        }
        SYNC;
        idx = 100;
        if (chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE) {
            chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE = 1;
            while (chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE && (idx > 0)) {
                nanospin_ns(1000);
                idx--;
            }
            if (idx == 0) {
                fprintf(stderr, "%s: DONE bit clear failed\n", __FUNCTION__);
            }
        }
        chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR = 1;
        chn_info_ptr->chn_regs_ptr->TCD = *chn_info_ptr->tcd_buffer; /* Copy first TCD to the TCD_SRAM */
        /* Configure SBR Attributes */
        imx_edma_set_bus_attr(chn_info_ptr, src_phy_mask, dst_phy_mask, tinfo->src_flags, tinfo->dst_flags);
        res = 0;
    } while (0);

    status = pthread_mutex_unlock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex unlock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }
    chn_info_ptr->tcd_last_sda_idx = 0;
    print_method_end_msg();

    if (res == -1) {
        errno = status;
    }

    return res;
}

/**
 * Start transfer.
 *
 * @param handle Channel handle.
 *
 * @return returns 0 on success or -1 on error(errno is set).
 */
static int imx_edma_xfer_start(void *const handle)
{
    chn_info_t         *chn_info_ptr = handle;
    int i = 100;
    int status;

    print_method_start_msg();

    status = pthread_mutex_lock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex lock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }

    atomic_clr(&chn_info_ptr->error_status, 0xFFFFFFFF);                /* Clear local copy of error status */
    chn_info_ptr->chn_regs_ptr->DMA_CHN_ES.B.ERR = 1;                   /* Clear ERROR bit (bit is W1C) */

    if (chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE) {
        chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE = 1;                 /* Clear DONE bit (bit is W1C) */
        while (chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE && (i > 0)) {
            nanospin_ns(1000);
            i--;
        }
        if (i == 0) {
            fprintf(stderr, "%s: DONE bit clear failed\n", __FUNCTION__);
        }
    }
    /* Initialize tcd_last_sda buffer */
    memset(chn_info_ptr->tcd_last_sda, 0, sizeof(uint32_t) * chn_info_ptr->tcd_cnt);
    /* Reload TCD from buffer */
    if (chn_info_ptr->tcd_reload) {
        chn_info_ptr->tcd_last_sda_idx = 0;
        chn_info_ptr->chn_regs_ptr->TCD = *chn_info_ptr->tcd_buffer; /* Copy first TCD to the TCD_SRAM */
    }
    chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.EEI = 1;  /* Enable error interrupt */

    print_channel_regs(chn_info_ptr->chn_regs_ptr);
    if (chn_info_ptr->request_source & IMX_DMA_REQ_SW) {
        print("SW DMA request, Setting START = 1 \n");
        chn_info_ptr->chn_regs_ptr->TCD.DMA_TCDn_CSR.B.START = 1;       /* Start transfer explicitly by SW */
    } else {
        print("HW DMA request, Setting ERQ = 1\n");
        chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.ERQ = 1;              /* Enable external request from peripheral */
    }
    print_channel_regs(chn_info_ptr->chn_regs_ptr);
    print_chn_info(chn_info_ptr);

    status = pthread_mutex_unlock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex unlock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }

    print_method_end_msg();
    return 0;
}

/**
 * Abort transfer.
 *
 * @param handle Channel handle.
 *
 * @return  0 on success or -1 on error(errno is set)
 */
static int imx_edma_xfer_abort(void *const handle)
{
    unsigned i = 0;
    int status;
    chn_info_t *chn_info_ptr = handle;
    print_method_start_msg();

    status = pthread_mutex_lock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex lock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }

    chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.ERQ = 0;  /* Disable external request from peripheral */
    chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.EEI = 0;  /* Disable error interrupt */
    /* Wait until minor loop is finished */
    while ((chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.ACTIVE) && (i < IMX_EDMA_ABORT_TIMEOUT_US)) {
        nanospin_ns(1000);
        i++;
    }

    if (i == IMX_EDMA_ABORT_TIMEOUT_US) {
        fprintf(stderr, "%s: DMA channel is ACTIVE\n", __FUNCTION__);
    }

    if (chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE) {
        /* Major loop is done, all data has been already transferred */
        print("major loop is done\n");
    } else {
        /* Next minor loop is in progress */
        print("next minor loop in progress\n");
    }

    print_channel_regs(chn_info_ptr->chn_regs_ptr);

    status = pthread_mutex_unlock(&chn_info_ptr->lock);
    if (status != EOK) {
        fprintf(stderr, "%s: Mutex unlock failed with status %u\n", __FUNCTION__, status);
        return -1;
    }

    print_method_end_msg();
    return 0;
}

/**
 * Transfer complete function.
 * @param handle Channel handle.
 *
 * @return 0 on success or -1 on errors(errno is set).
 */
static int imx_edma_xfer_complete(void *const handle)
{
    const chn_info_t  *const chn_info_ptr = handle;

    print_method_start_msg();
    print_channel_regs(chn_info_ptr->chn_regs_ptr);
    print_method_end_msg();
    print("chn_info_ptr->error_status: 0x%08x\n", status);

    if (chn_info_ptr->error_status) {
        errno = EIO;
        return -1;
    }

    return 0;
}

/**
 * Init function. For backward compatibility only.
 *
 * @param options Driver input options.
 *
 * @return Always return 0.
 */
static int imx_edma_init(__attribute__((unused)) const char *const options)
{
    return 0;
}

/**
 * Fini function. For backward compatibility only.
 */
static void imx_edma_fini(void)
{
}

/**
 * Gets actual number of transfered bytes.
 *
 * When a channel completed a major loop and DONE bit is set the tcd_last_sda is set by eDMA and contains address of the last destination address. We cannot
 * read destination address from TCD register directly because TCD is automatically reloaded. We can only calculate
 * bytes transfered to the destination so it is usable for receivers using DMA, eg. UART RX.
 *
 * Total amount of transfered bytes is not calculated correctly when DMA_MODE_FLAG_REPEAT flag is set for a channel.
 * In that case an amount of transfered bytes is calculated only for last iteration from a begin to an address
 * stored in the tcd_last_sda variable. To get information about bytes sent on every TCD please refer to sig_bytes_sent=1 option.
 *
 * @param handle Channel handle.
 *
 * @return Number of transfered bytes.
 */

/* Note:
 *   Changing behavious based on DONE bit causes a race condition where the TCD may transition to DONE while calculating the
 *   bytes_left in the second scenario above and therefore occasionally return an invalid bytes_left.
 *   Further, through observations occasionally additional interrupts are generated which causes the LPUART (the only user
 *   of this function now) to believe a transfer has completed when it hasn't causing the LPUART to switch ping/pong buffers
 *   while the transfer is progressing.
 *   Now we only calculte the bytes_left When DONE bit is set. If DONE bit is not set (as it should always be), this function
 *   would set an overrun error and return with zero.
 */
#define EDMA_SEGMENT_ERROR_STATUS_OVERRUN 0x80000400

static unsigned imx_edma_bytes_left(void *const handle)
{
    chn_info_t  *chn_info_ptr = handle;
    unsigned i, tcd_index;
    unsigned left = 0;

    if (!chn_info_ptr->chn_regs_ptr->DMA_CHN_CSR.B.DONE) {
        atomic_set(&chn_info_ptr->error_status, EDMA_SEGMENT_ERROR_STATUS_OVERRUN);
        return 0;
    }

    tcd_index = chn_info_ptr->tcd_cnt;

    /* Calculate transfered bytes for finished TCD descriptors */
    for (i = 0; (i < tcd_index) && (chn_info_ptr->tcd_last_sda[i] != 0); i++) {
        /* Calculation is supported when DADDR is incremented so we can use tcd_last_sda */
        if (chn_info_ptr->tcd_buffer[i].DMA_TCDn_DOFF.R != 0) {
            left += chn_info_ptr->tcd_last_sda[i] - chn_info_ptr->tcd_buffer[i].DMA_TCDn_DADDR.R;
        }
    }
    return left;
}

/**
 * Initialize function pointer table.
 *
 * @param functable Function pointer table address.
 * @param tabsize   Function pointer table size.
 *
 * @return Always returns 0.
 */
int get_dmafuncs(dma_functions_t *funcs, const int tabsize)
{
    DMA_ADD_FUNC(funcs, driver_info,     imx_edma_driver_info, tabsize);
    DMA_ADD_FUNC(funcs, channel_info,    imx_edma_channel_info, tabsize);
    DMA_ADD_FUNC(funcs, channel_attach,  imx_edma_channel_attach, tabsize);
    DMA_ADD_FUNC(funcs, channel_release, imx_edma_channel_release, tabsize);
    DMA_ADD_FUNC(funcs, alloc_buffer,    imx_dma_alloc_buffer, tabsize);
    DMA_ADD_FUNC(funcs, free_buffer,     imx_dma_free_buffer, tabsize);
    DMA_ADD_FUNC(funcs, setup_xfer,      imx_edma_setup_xfer, tabsize);
    DMA_ADD_FUNC(funcs, xfer_start,      imx_edma_xfer_start, tabsize);
    DMA_ADD_FUNC(funcs, xfer_abort,      imx_edma_xfer_abort, tabsize);
    DMA_ADD_FUNC(funcs, xfer_complete,   imx_edma_xfer_complete, tabsize);
    DMA_ADD_FUNC(funcs, init,            imx_edma_init, tabsize);
    DMA_ADD_FUNC(funcs, fini,            imx_edma_fini, tabsize);
    DMA_ADD_FUNC(funcs, bytes_left,      imx_edma_bytes_left, tabsize);
    DMA_ADD_FUNC(funcs, query_channel,   imx_edma_query_channel, tabsize);
    DMA_ADD_FUNC(funcs, channel_attach_smmu,   imx_edma_channel_attach_smmu, tabsize);
    return 0;
}

/* *****************************************************************************
 *                            PUBLIC FUNCTIONS                                 *
 ******************************************************************************/

static void ctor(void) __attribute__((__constructor__));
static void dtor(void) __attribute__((__destructor__));

/**
 * EDMA library constructor. It's run when a shared library is loaded.
 */
static void ctor(void)
{
    uint32_t         i, j;
    rsrc_alloc_t     ralloc;
    unsigned         hwi_off;
    int              units;
    edma_device_info_t *ptr = NULL;
    unsigned         idx;
    unsigned         next_dma_chnl_offset = 0;

    units = hwi_find_num_units(IMX_HWI_DMA);
    if (units > 0) {
        ptr = calloc(1, sizeof(edma_device_info_t) * units);
        edma_device_info = ptr;
        edma_device_count = (unsigned)units;
        if (!ptr) {
            perror("EDMA ctor() calloc failed\n");
            return;
        }
        edma_chn_hi = 0;
        for(i = 0; i < (unsigned int)units; i++) {
            hwi_off = hwi_find_device(IMX_HWI_DMA, i);
            if (hwi_off != HWI_NULL_OFF) {
                const hwi_tag *const tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, NULL);
                if (tag) {
                    ptr->base = (uint32_t)tag->location.base;
                    if (ptr->base == 0) {
                        perror("eDMA ctor() Unable to obtain base address\n");
                    }
                }
                idx = 0;
                ptr->irq = (int)hwitag_find_ivec(hwi_off, &idx);
                if (ptr->irq == (int)HWI_ILLEGAL_VECTOR) {
                    perror("eDMA ctor() Unable to obtain IRQ\n");
                }
                ptr->error_irq = (int)hwitag_find_ivec(hwi_off, &idx);
                if (ptr->irq == (int)HWI_ILLEGAL_VECTOR) {
                    perror("eDMA ctor() Unable to obtain IRQ\n");
                }
                ptr->chn_base = ptr->base + IMX_DMA_DEVICE_SIZE;
                /* Calc. channel count */
                j = 8;
                while((hwi_tag_find(hwi_off, HWI_TAG_NAME_dma, &j) != 0)) {
                    j += 7;
                }
                ptr->chn_num = j;
                ptr->resdb_offset = next_dma_chnl_offset;
                /* Calculate channel offset for next eDMA instance because we operate with 32 channels per eDMA */
                next_dma_chnl_offset += 32 - ptr->chn_num;
                edma_chn_hi +=  ptr->chn_num;
                ptr->errata = (unsigned int)hwitag_find_errata(hwi_off, NULL);
                print("base 0x%x irq %u err_irq %u chn_num %u errata %u\n", ptr->base, ptr->irq, ptr->error_irq, ptr->chn_num, ptr->errata);

                ptr++;
            } else {
                perror("cannot find dma device in hw table\n");
            }
        }
        /* Seed the resource db manager. */
        edma_chn_hi -= 1; // Highest channel is channel count - 1
        memset(&ralloc, 0, sizeof(ralloc));
        ralloc.start    = edma_chn_lo;
        ralloc.end      = edma_chn_hi;
        ralloc.flags    = RSRCDBMGR_DMA_CHANNEL | RSRCDBMGR_FLAG_NOREMOVE;
        if (rsrcdbmgr_create(&ralloc, 1) != 0) {
            perror("eDMA ctor() Unable to seed dma channels\n");
        }
    }
}

static void dtor(void)
{
    if (edma_device_info) {
        free(edma_device_info);
        edma_device_info = NULL;
    }
}
/** @} */ /* End of edma */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/edma/edma.c $ $Rev: 970391 $")
#endif
