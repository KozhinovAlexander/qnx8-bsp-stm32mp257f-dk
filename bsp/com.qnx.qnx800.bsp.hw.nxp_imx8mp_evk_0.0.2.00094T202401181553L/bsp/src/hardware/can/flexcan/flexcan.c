/*
 * Copyright (c) 2020, 2021, 2023, BlackBerry Limited.
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

#include "flexcan.h"

/* Set default CAN message ID's in a range that is valid for both standard and extended ID's */
#define FLEXCAN_MID_DEFAULT     (0x00100000u)
#define FLEXCAN_INTBIT(mbxid)   (1u << (mbxid))

static void flexcan_tx(flexcan_t* const cdev, canmsg_t* const txmsg);
static void flexcan_debug(flexcan_t* const dev);

/**
 *  @brief              Read modify write FLEXCAN register
 *  @param  port        Register base
 *  @param  mask        Data mask
 *  @param  data        Data value
 *
 *  @return             none
 */
static void set_port32(const uintptr_t port, const uint32_t mask, const uint32_t data)
{
    out32(port, (in32(port) & ~mask) | (data & mask));
}

/**
 *  @brief              Read FLEXCAN register with mask
 *  @param  port        Register base
 *  @param  mask        Data mask
 *
 *  @return             Read result
 */
static uint32_t get_port32(const uintptr_t port, const uint32_t mask)
{
    return (in32(port) & mask);
}

/**
 *  @brief              Convert from DLC to message length
 *  @param  dlc         Data length code
 *
 *  @return             Data length
 */
static uint8_t flexcan_dlc2len(const uint8_t dlc)
{
#define MAX_DLC 16
    uint8_t const msglen[MAX_DLC] = { 0, 0, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64 };

    if (dlc >= MAX_DLC) return 0;

    return msglen[dlc];
}

/**
 *  @brief              Convert from message length to DLC
 *  @param  mlen        Message length
 *
 *  @return             Data length code
 */
static uint8_t flexcan_len2dlc(const uint8_t dlc)
{
    if (dlc <= 1) return 0;
    else if (dlc <= 8) return dlc;
    else if (dlc <= 12) return 9;
    else if (dlc <= 16) return 10;
    else if (dlc <= 20) return 11;
    else if (dlc <= 24) return 12;
    else if (dlc <= 32) return 13;
    else if (dlc <= 48) return 14;
    else return 15;
}


/**
 *  @brief              Get FLEXCAN mailbox
 *  @param  devinfo     FLEXCAN device info structure
 *  @param  mbxid       FLEXCAN mail box ID
 *
 *  @return             Pointer to mailbox structure --success
 *                      NULL --failure
 */
static can_msg_obj_t *flexcan_get_mb(flexcan_info_t* const devinfo, const uint32_t mbxid)
{
    can_msg_obj_t   *mb;

    if (mbxid > devinfo->num_mailboxes) return NULL;

    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        uint32_t bank_id, id;

        bank_id = mbxid / 7;
        id = mbxid % 7;

        mb = (can_msg_obj_t *)(bank_id * 0x200 + id * sizeof(canfd_msg_obj_t) + (uintptr_t)devinfo->canmsg);
    } else {
        mb = &devinfo->canmsg[mbxid];
    }

    return mb;
}

/**
 *  @brief              Set FLEXCAN bit timing
 *  @param  devinfo     FLEXCAN device info structure
 *  @param  devinit     FLEXCAN device init structure
 *
 *  @return             NULL
 */
static void flexcan_set_bittiming(flexcan_info_t* const devinfo, const flexcan_init_t* const devinit)
{
    uint32_t reg_ctrl = 0;

    reg_ctrl = in32(devinfo->base + FLEXCAN_CTRL);

    if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
        uint32_t reg_cbt = 0, reg_fdcbt = 0;

        reg_ctrl |= CTRL_LPB | CTRL_LOM | CTRL_SAMP;

        /* CAN FD */
        reg_cbt = CBT_EPRESDIV(devinit->bt.brp - 1) |
            CBT_EPSEG1(devinit->bt.phase_seg1 - 1) |
            CBT_EPSEG2(devinit->bt.phase_seg2 - 1) |
            CBT_ERJW(devinit->bt.sjw - 1) |
            CBT_EPROPSEG(devinit->bt.prop_seg - 1) |
            CBT_BTF;

        out32(devinfo->base + FLEXCAN_CBT, reg_cbt);

        reg_fdcbt = FDCBT_FPRESDIV(devinit->dbt.brp - 1) |
            FDCBT_FPSEG1(devinit->dbt.phase_seg1 - 1) |
            FDCBT_FPSEG2(devinit->dbt.phase_seg2 - 1) |
            FDCBT_FRJW(devinit->dbt.sjw - 1) |
            FDCBT_FPROPSEG(devinit->dbt.prop_seg);

        out32(devinfo->base + FLEXCAN_FDCBT, reg_fdcbt);
    } else {
        /* Legacy CAN */
        reg_ctrl &= ~(CTRL_PRESDIV(0xff) |
                CTRL_RJW(0x3) |
                CTRL_PSEG1(0x7) |
                CTRL_PSEG2(0x7) |
                CTRL_PROPSEG(0x7));

        reg_ctrl |= CTRL_PRESDIV(devinit->bt.brp - 1) |
                CTRL_PSEG1(devinit->bt.phase_seg1 - 1) |
                CTRL_PSEG2(devinit->bt.phase_seg2 - 1) |
                CTRL_RJW(devinit->bt.sjw - 1) |
                CTRL_PROPSEG(devinit->bt.prop_seg - 1);
    }

    out32(devinfo->base + FLEXCAN_CTRL, reg_ctrl);
}

/**
 *  @brief              Handle FLEXCAN mailbox Rx interrupt
 *  @param  devinfo     FLEXCAN device info structure
 *  @param  mbxid       Mailbox ID
 *
 *  @return             Pointer to CANDEV --event occurred for CANDEV
 *                      NULL --No event
 */
static CANDEV *flexcan_rx_intr(flexcan_info_t* const devinfo, const uint32_t mbxid)
{
    flexcan_t       *devlist = devinfo->devlist;
    canmsg_t        *rxmsg = NULL;
    uint32_t        ctrl, canmcf_ide = 0;
    uint8_t         dlc;
    canfd_msg_obj_t *canmsg;

    canmsg = (canfd_msg_obj_t *)flexcan_get_mb(devinfo, mbxid);
    if (canmsg == NULL) {
        can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbxid);
        return NULL;
    }

    /* Get the next free receive message or overwrite the oldest */
    rxmsg = canmsg_dequeue_element(devlist[mbxid].cdev.free_queue);
    /* if no queue elements are free, re-use the oldest rx message */
    if (rxmsg == NULL) {
        rxmsg = canmsg_dequeue_element(devlist[mbxid].cdev.msg_queue);
        devinfo->stats.sw_receive_q_full++;
        if (rxmsg == NULL) {
            /*
             * This should not happen, at lease one of the queue should have a message buffer
             * Simply return NULL to make the code look resonable
             */
            return NULL;
        }
    }

    /*
     * Reading the canmcf control status word of the message mbx
     * buffer triggers a lock for that buffer.  It stays locked until
     * the free running timer is read, preventing corruption issues
     * due to a new message filling the mailbox.  A shadow register
     * takes care of preserving new messages.
     */

    /* Make sure that the mailbox is not busy- CPU access is not permitted while busy.*/
    do {
        ctrl = canmsg->canmcf;
    } while ((ctrl & (REC_CODE_BUSY << 24)) != 0);
    /* Retrieve the message length from the DLC field */
    dlc = (uint8_t)((ctrl & MSG_BUF_DLC_MASK) >> MSG_BUF_DLC_SHIFT);
    rxmsg->cmsg.len = flexcan_dlc2len(dlc);

    /* Save the message ID */
    rxmsg->cmsg.mid = canmsg->canmid;
    canmcf_ide = ctrl & MB_CNT_IDE;

    rxmsg->cmsg.ext.is_extended_mid = canmcf_ide > 0;

    /*
     *  Access the data as a uint32_t array for endian conversion
     *  and copy data from receive mailbox to receive message
     */
    uint32_t    *dst32 = (uint32_t *)rxmsg->cmsg.dat;
    const uint32_t *src32 = (uint32_t *)canmsg->canmd;
    for (uint8_t idx = 0; idx < rxmsg->cmsg.len; idx += 4) {
        *dst32 = *src32;
        if (devinfo->flags & FLEXCAN_FLAGS_ENDIAN_SWAP) {
            ENDIAN_SWAP32(dst32);
        }
        dst32++;
        src32++;
    }

    /* Set mailbox to empty */
    canmsg->canmcf = ((canmsg->canmcf & ~MSG_BUF_CODE_MASK) |
                     (REC_CODE_EMPTY << MSG_BUF_CODE_SHIFT) | canmcf_ide);

    /* Unlock the message buffer by reading the CAN free running timer */
    devinfo->timer = in32(devinfo->base + FLEXCAN_TIMER);

    /* Add message timestamp */
    {
        /* Save free-running timer as message timestamp. */
        rxmsg->cmsg.ext.timestamp = ctrl & 0x0000FFFF;
    }

    /* Add populated element to the receive queue */
    canmsg_queue_element(devlist[mbxid].cdev.msg_queue, rxmsg);
    devinfo->stats.received_frames++;

    /* return CANDEV to event handler */
    return &devlist[mbxid].cdev;
}

/**
 *  @brief              Handle FLEXCAN memory error interrupt event
 *  @param  devinfo     FLEXCAN device info structure
 *
 *  @return             None
 */
static void flexcan_mec_intr(flexcan_info_t* const devinfo)
{
    const uintptr_t mecbase = devinfo->base + FLEXCAN_MECR;
    const unsigned errsr = in32(mecbase + FLEXCAN_ERRSR);
    uint32_t flags = 0;

    can_slogf(_SLOG_DEBUG1, "%s: MECR ERRSR 0x%X", __func__, errsr);

    if (errsr == 0) {
        return;
    }

    if (errsr & FLEXCAN_ERRSR_CEIOF) {
        flags |= FLEXCAN_ERRSR_CEIOF;
    }
    if (errsr & FLEXCAN_ERRSR_FANCEIOF) {
        flags |= FLEXCAN_ERRSR_FANCEIOF;
    }
    if (errsr & FLEXCAN_ERRSR_HANCEIOF) {
        flags |= FLEXCAN_ERRSR_HANCEIOF;
    }
    if (errsr & FLEXCAN_ERRSR_CEIF) {
        flags |= FLEXCAN_ERRSR_CEIF;
    }
    if (errsr & FLEXCAN_ERRSR_FANCEIF) {
        flags |= FLEXCAN_ERRSR_FANCEIF;
    }
    if (errsr & FLEXCAN_ERRSR_HANCEIF) {
        flags |= FLEXCAN_ERRSR_HANCEIF;
    }

    out32(mecbase + FLEXCAN_ERRSR, flags);

    if (in32(devinfo->base + FLEXCAN_MCR) & MCR_HALT){
        // Take FlexCAN out of freeze mode and continue
        set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, 0);
    }
}

/**
 *  @brief              Handle FLEXCAN mailbox interrupt event
 *  @param  devinfo     FLEXCAN device info structure
 *
 *  @return             Pointer to CANDEV --event occurred for CANDEV
 *                      NULL --No event
 */
static CANDEV *flexcan_mb_intr(flexcan_info_t* const devinfo)
{
    flexcan_t   *devlist = devinfo->devlist;
    uint32_t    irqsrc1 = 0, irqsrc2 = 0;
    uint32_t    irqsrc3 = 0, irqsrc4 = 0;
    CANDEV      *cdev;

    irqsrc1 = in32(devinfo->base + FLEXCAN_IFLAG1);
    can_slogf(_SLOG_DEBUG1, "%s: IRQ status 1 %x", __func__, irqsrc1);

    /* RAW mode */
    if (devinfo->mode == CANDEV_MODE_RAW_FRAME) {
        if (irqsrc1 == 0) return NULL;

        /* Get the id of the mailbox that generated the interrupt */
        if (irqsrc1 & RAW_MODE_RX_IRQ) {
            const uint32_t mbxid = (ffs((int)(irqsrc1 & RAW_MODE_RX_IRQ)) - 1);
            cdev = flexcan_rx_intr(devinfo, mbxid);
            out32(devinfo->base + FLEXCAN_IFLAG1, 1u << mbxid);
            if (cdev != NULL) return cdev;
        }
        if (irqsrc1 & RAW_MODE_TX_IRQ) {
            const uint32_t mbxid = (ffs((int)(irqsrc1 & RAW_MODE_TX_IRQ)) - 1);
            out32(devinfo->base + FLEXCAN_IFLAG1, 1u << mbxid);
            devlist[mbxid].dflags &= ~CANDEV_TX_ENABLED;
            devinfo->stats.transmitted_frames++;

            return &devlist[mbxid].cdev;
        }

        /* Just in case there are unwanted interrupts */
        out32(devinfo->base + FLEXCAN_IFLAG1, irqsrc1 & ~(RAW_MODE_RX_IRQ | RAW_MODE_TX_IRQ));

        return NULL;
    }

    /* CANFD, only use IRQ status 1 */
    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        if (irqsrc1 == 0) return NULL;

        for (uint32_t mbx = 0; mbx < devinfo->num_mailboxes; mbx++) {
            if (irqsrc1 & (1u << mbx)) {
                /* Clear this IRQ */
                out32(devinfo->base + FLEXCAN_IFLAG1, 1u << mbx);
                if (mbx < devinfo->numrx) {
                    return flexcan_rx_intr(devinfo, mbx);
                } else {
                    devlist[mbx].dflags &= ~CANDEV_TX_ENABLED;
                    devinfo->stats.transmitted_frames++;

                    return &devlist[mbx].cdev;
                }
            }
        }
        /* Just in case */
        out32(devinfo->base + FLEXCAN_IFLAG1, irqsrc1);

        return NULL;
    }

    irqsrc2 = in32(devinfo->base + FLEXCAN_IFLAG2);     /* for Rx32-63 or Tx0-31 */
    can_slogf(_SLOG_DEBUG1, "%s: IRQ status 2 %x", __func__, irqsrc2);
    if (devinfo->numrx > 32) {
        irqsrc3 = in32(devinfo->base + FLEXCAN_IFLAG3); /* for Tx0-31  */
        irqsrc4 = in32(devinfo->base + FLEXCAN_IFLAG4); /* for Tx32-63 */
        can_slogf(_SLOG_DEBUG1, "%s: IRQ status 3 %x", __func__, irqsrc3);
        can_slogf(_SLOG_DEBUG1, "%s: IRQ status 4 %x", __func__, irqsrc4);
    }

    if ((irqsrc1 == 0) && (irqsrc2 == 0) &&
        (irqsrc3 == 0) && (irqsrc4 == 0)) return NULL;

    /* Either 32 or 64 Rx mailboxes
     * irqsrc2 is for Rx32-63 in case of 64 Rx mailboxes
     */
    for (uint32_t mbx = 0; mbx < devinfo->numrx; mbx++) {
        if (mbx < 32) {
            if (irqsrc1 & (1u << mbx)) {
                cdev = flexcan_rx_intr(devinfo, mbx);
                /* Clear this IRQ */
                out32(devinfo->base + FLEXCAN_IFLAG1, 1u << mbx);
                if (cdev != NULL) return cdev;
            }
        } else {
            if (irqsrc2 & (1u << (mbx - 32))) {
                cdev = flexcan_rx_intr(devinfo, mbx);
                /* Clear this IRQ */
                out32(devinfo->base + FLEXCAN_IFLAG2, 1u << (mbx - 32));
                if (cdev != NULL) return cdev;
            }
        }
    }

    /* Either 32 or 64 Tx mailboxes
     * irqsrc2 is for Tx0-31 in case of 32 Rx mailboxes
     * irqsrc3 is for Tx0-31 in case of 64 Rx mailboxes
     * irqsrc4 is for Tx32-63 in case of 64 Rx mailboxes
     */
    for (uint32_t mbx = 0; mbx < devinfo->numtx; mbx++) {
        if (devinfo->numrx <= 32) {
            if (irqsrc2 & (1u << mbx)) {    /* irqsrc2 for Tx0-31 */
                /* Clear this IRQ */
                out32(devinfo->base + FLEXCAN_IFLAG2, 1u << mbx);
                devlist[mbx + devinfo->numrx].dflags &= ~CANDEV_TX_ENABLED;
                devinfo->stats.transmitted_frames++;

                return &devlist[mbx + devinfo->numrx].cdev;
            }
        } else {    /* irqsrc3 for Tx0-31, irqsrc4 for Tx32-63 */
            if (mbx < 32) {
                if (irqsrc3 & (1u << mbx)) {
                    /* Clear this IRQ */
                    out32(devinfo->base + FLEXCAN_IFLAG3, 1u << mbx);
                    devlist[mbx + devinfo->numrx].dflags &= ~CANDEV_TX_ENABLED;
                    devinfo->stats.transmitted_frames++;

                    return &devlist[mbx + devinfo->numrx].cdev;
                }
            } else {
                if (irqsrc4 & (1u << (mbx - 32))) {
                    /* Clear this IRQ */
                    out32(devinfo->base + FLEXCAN_IFLAG4, (1u << (mbx - 32)));
                    devlist[mbx + devinfo->numrx].dflags &= ~CANDEV_TX_ENABLED;
                    devinfo->stats.transmitted_frames++;

                    return &devlist[mbx + devinfo->numrx].cdev;
                }
            }
        }
    }

    if (irqsrc1 != 0) {
        can_slogf(_SLOG_WARNING, "%s: Unknown IRQ status 1 %x", __func__, irqsrc1);
        out32(devinfo->base + FLEXCAN_IFLAG1, irqsrc1);
    }
    if (irqsrc2 != 0) {
        can_slogf(_SLOG_WARNING, "%s: Unknown IRQ status 2 %x", __func__, irqsrc2);
        out32(devinfo->base + FLEXCAN_IFLAG2, irqsrc2);
    }
    if (irqsrc3 != 0) {
        can_slogf(_SLOG_WARNING, "%s: Unknown IRQ status 3 %x", __func__, irqsrc3);
        out32(devinfo->base + FLEXCAN_IFLAG3, irqsrc3);
    }
    if (irqsrc4 != 0) {
        can_slogf(_SLOG_WARNING, "%s: Unknown IRQ status 4 %x", __func__, irqsrc4);
        out32(devinfo->base + FLEXCAN_IFLAG4, irqsrc4);
    }

    return NULL;
}

/**
 *  @brief              FLEXCAN interrupt event handler
 *  @param  hdl         FLEXCAN device info handle
 *  @param  pulse_code  Event pulse code
 *
 *  @return             Pointer to CANDEV --event occurred for CANDEV
 *                      NULL --No event
 */
CANDEV* flexcan_event_handler(void* const hdl, const int pulse_code)
{
    flexcan_info_t  *devinfo = hdl;
    uint32_t        estat;
    static int      erroractive = 1;
    CANDEV*         cdev = NULL;

    devinfo->stats.total_interrupts++;

    /* Check for System and Error Interrupts - log the error and clear
       the interrupt source. */
    estat = in32(devinfo->base + FLEXCAN_ESR);
    can_slogf(_SLOG_DEBUG1, "%s: error status %x", __func__, estat);
    if (estat != 0) {
        /* One of the following can actually occur:
        -> BusOff.
        -> ErrInt - in this case indicates that at least one of the Error Bits (bits 15-10) is set.
        -> FCS - Fault Confinment State tells us
             00 - all ok, error active mode
             01 - error passive mode
             1x - Bus off
        -> RX warning level reached
        -> Tx warning level reached
        -> WakeUp Interrupt

        Most bits in CANESR are read only, except TWRN_INT, RWRN_INT, BOFF_INT, WAK_INT and
        ERR_INT, which are interrupt flags that can be cleared by writing 1 to them.

        If we have an error interrupt, reset all error conditions, we have saved them in estat
         for processing later on, so that we can move this error handling at the end of the ISR
         to have better RX response times.

         BusOff disables the Interrupt generation itself by resetting
         the mask in CANCTRL.

         We do reset all possible interrupts and look what we got later.
        */

        /* Tx Warning Level Interrupt Flag */
        if (estat & ESR_TWRNINT) {
            devinfo->stats.error_warning_state_count++;
            /* Store error to be retrieved by devctl */
            devinfo->canestat |= ESR_TWRNINT;
            /* Clear interrupt source */
            out32(devinfo->base + FLEXCAN_ESR, ESR_TWRNINT);
        }
        /* Rx Warning Level Interrupt Flag */
        if (estat & ESR_RWRNINT) {
            devinfo->stats.error_warning_state_count++;
            /* Store error to be retrieved by devctl */
            devinfo->canestat |= ESR_RWRNINT;
            /* Clear interrupt source */
            out32(devinfo->base + FLEXCAN_ESR, ESR_RWRNINT);
        }
        /* Error Interrupt Flag */
        if (estat & ESR_ERRINT) {
            /* Store error to be retrieved by devctl */
            devinfo->canestat |= ESR_ERRINT;
            if (estat & ESR_BITERR_RECESSIVE_DOMINANT) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_BITERR_RECESSIVE_DOMINANT;
                devinfo->stats.recess_bit_dom_errors++;
                devinfo->stats.total_frame_errors++;
            }
            if (estat & ESR_BITERR_DOMINANT_RECESSIVE) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_BITERR_DOMINANT_RECESSIVE;
                devinfo->stats.dom_bit_recess_errors++;
                devinfo->stats.total_frame_errors++;
            }
            if (estat & ESR_ACKERR) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_ACKERR;
                devinfo->stats.missing_ack++;
                devinfo->stats.total_frame_errors++;
            }
            if (estat & ESR_CRCERR) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_CRCERR;
                devinfo->stats.crc_errors++;
                devinfo->stats.total_frame_errors++;
            }
            if (estat & ESR_FORMERR) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_FORMERR;
                devinfo->stats.form_errors++;
                devinfo->stats.total_frame_errors++;
            }
            if (estat & ESR_STUFFERR) {
                /* Store error to be retrieved by devctl */
                devinfo->canestat |= ESR_STUFFERR;
                devinfo->stats.stuff_errors++;
                devinfo->stats.total_frame_errors++;
            }
            /* Clear interrupt source */
            out32(devinfo->base + FLEXCAN_ESR, ESR_ERRINT);
        }
        /* Bus-Off Interrupt Flag */
        if (estat & ESR_BOFFINT) {
            /* Store error to be retrieved by devctl */
            devinfo->canestat |= ESR_BOFFINT;
            devinfo->stats.bus_off_state_count++;
            devinfo->stats.total_frame_errors++;

            /* Clear interrupt source */
            out32(devinfo->base + FLEXCAN_ESR, ESR_BOFFINT);
        }
        /* Wakeup-Up Interrupt Flag */
        if (estat & ESR_WAKEINT) {
            /* Store error to be retrieved by devctl */
            devinfo->canestat |= ESR_WAKEINT;
            devinfo->stats.wake_up_count++;
            /* Clear interrupt source */
            out32(devinfo->base + FLEXCAN_ESR, ESR_WAKEINT);
        }
        /* FCS - the Fault Confinement State
          *   if Bit4 (ESR_FCS_ERROR_PASSIVE) is set :  Error Passive
          *   else Error Active.
          */
        /* Going back to Error Active can happen without an error Int ? */
        if ((estat & ESR_FCS_MASK) == 0) {
            if (!erroractive) {
            /* Going error active */
            }
            erroractive = 1;
        }

        if ((estat & ESR_FCS_MASK) == ESR_FCS_ERROR_PASSIVE) {
            if (erroractive) {
                /* Going error passive */
                devinfo->stats.error_passive_state_count++;
            }
            erroractive = 0;
        }

        /* Clear Out Interrupts */
        out32(devinfo->base + FLEXCAN_ESR, estat);
    }

    /* Check for Mailbox Interrupts */
    cdev = flexcan_mb_intr(devinfo);

    if (devinfo->flags & FLEXCAN_FLAGS_DISABLE_MECR) {
        /* The MECR interrupts have been disabled, but FLEXCAN_ERRSR
         * will still be set.  Clear the flags. */
        flexcan_mec_intr(devinfo);
    }

    /* Return to resource manager if valid interrupt detected */
    if (cdev != NULL) return cdev;

    /* Only unmask the interrupt after all interrupts are processed
     * resource manager will not call event handler again once NULL is returned.
     */
    InterruptUnmask(devinfo->irqsys[pulse_code], devinfo->iidsys[pulse_code]);

    return NULL;
}

/**
 *  @brief              LIBCAN driver transmit function
 *  @param  CANDEV      CANDEV handle
 *
 *  @return             None
 */
void flexcan_transmit(CANDEV* const cdev)
{
    flexcan_t   *dev = (flexcan_t *)cdev;
    canmsg_t    *txmsg;

    /* Make sure transmit isn't already in progress and there is valid data */
    if (!(dev->dflags & CANDEV_TX_ENABLED)) {
        txmsg = canmsg_dequeue_element(cdev->msg_queue);
        if (txmsg != NULL) {
            /* Indicate transmit is in progress */
            dev->dflags |= CANDEV_TX_ENABLED;
            /* Start transmission */
            flexcan_tx(dev, txmsg);
            /* tx message is copied - return element to free queue */
            canmsg_queue_element(cdev->free_queue, txmsg);
        }
    }
}

/**
 *  @brief              Set/clear interrupt mask bit
 *  @param  devinfo     Pointer to FLEXCAN device info structure
 *  @param  mbxid       Mailbox ID
 *  @param  setv        Set mask bit
 *
 *  @return             None
 */
static void flexcan_set_imask(const flexcan_info_t* const devinfo, const uint32_t mbxid, const int setv)
{
    const uint32_t canimask[] = { FLEXCAN_IMASK1, FLEXCAN_IMASK2, FLEXCAN_IMASK3, FLEXCAN_IMASK4 };
    uint32_t num, id;

    num = mbxid / 32;
    id  = mbxid - (num * 32);

    if (num >= (uint32_t)(sizeof(canimask) / sizeof(canimask[0]))) return;

    if (setv == 0) {
       set_port32(devinfo->base + canimask[num], FLEXCAN_INTBIT(id), 0);
    } else {
       set_port32(devinfo->base + canimask[num], FLEXCAN_INTBIT(id), FLEXCAN_INTBIT(id));
    }
}

/**
 *  @brief              Clear interrupt status bit
 *  @param  devinfo     Pointer to FLEXCAN device info structure
 *  @param  mbxid       Mailbox ID
 *
 *  @return             None
 */
static void flexcan_clear_iflag(const flexcan_info_t* const devinfo, const uint32_t mbxid)
{
    const uint32_t caniflag[] = { FLEXCAN_IFLAG1, FLEXCAN_IFLAG2, FLEXCAN_IFLAG3, FLEXCAN_IFLAG4 };
    uint32_t num, id;

    num = mbxid / 32;
    id  = mbxid - (num * 32);

    set_port32(devinfo->base + caniflag[num], FLEXCAN_INTBIT(id), FLEXCAN_INTBIT(id));
}

/**
 *  @brief              Get interrupt status
 *  @param  devinfo     Pointer to FLEXCAN device info structure
 *  @param  mbxid       Mailbox ID
 *
 *  @return             Interrupt status bit for this mailbox
 */
static uint32_t flexcan_get_iflag(flexcan_info_t* const devinfo, const uint32_t mbxid)
{
    const uint32_t caniflag[] = { FLEXCAN_IFLAG1, FLEXCAN_IFLAG2, FLEXCAN_IFLAG3, FLEXCAN_IFLAG4 };
    uint32_t num, id;

    num = mbxid / 32;
    id  = mbxid - (num * 32);

    return (get_port32(devinfo->base + caniflag[num], FLEXCAN_INTBIT(id)));
}

/**
 *  @brief              LIBCAN driver devctl function
 *  @param  cdev        CANDEV pointer
 *  @param  dcmd        devctl command
 *  @param  data        devctl data pointer
 *
 *  @return             Interrupt status bit for this mailbox
 */
int flexcan_devctl(CANDEV* const cdev, io_devctl_t *msg)
{
    flexcan_t* const dev = (flexcan_t *)cdev;
    flexcan_info_t  *devinfo = dev->devinfo;
    CAN_DCMD_DATA   *dcmd_data;
    const uint32_t   mbxid = dev->mbxid;
    int              timeout;
    uint32_t         cantest = 0, offset = 0;
    can_msg_obj_t   *canmsg;
    int              ret = 0;

    canmsg = flexcan_get_mb(devinfo, mbxid);
    if (canmsg == NULL) {
        can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbxid);
        errno = ENODEV;
        return -1;
    }

    switch (msg->i.dcmd) {
        case CAN_DEVCTL_SET_MID:
            if (devinfo->mode == CANDEV_MODE_RAW_FRAME) {
                errno = EINVAL;
                return -1;
            }
            dcmd_data = _IO_INPUT_PAYLOAD(msg);
            if (!(devinfo->flags & FLEXCAN_FLAGS_CANFD)) {
                /* Disable Object */
                flexcan_set_imask(devinfo, mbxid, 0);
                /* Set new message ID */
                canmsg->canmid = (canmsg->canmid & ~MID_MASK_EXT) | dcmd_data->mid;
                /* Enable Object */
                flexcan_set_imask(devinfo, mbxid, 1);
            } else {
                /* Disable Object */
                if (mbxid < devinfo->numrx) {
                    set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), 0);
                } else {
                    set_port32(devinfo->base + FLEXCAN_IMASK2, FLEXCAN_INTBIT(mbxid), 0);
                }
                /* Set new message ID */
                canmsg->canmid = (canmsg->canmid & ~MID_MASK_EXT) | dcmd_data->mid;
                /* Enable Object */
                if (mbxid < devinfo->numrx) {
                    set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), FLEXCAN_INTBIT(mbxid));
                } else {
                    set_port32(devinfo->base + FLEXCAN_IMASK2, FLEXCAN_INTBIT(mbxid), FLEXCAN_INTBIT(mbxid));
                }
            }
            break;

        case CAN_DEVCTL_GET_MID:
            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /* Read device's current message ID (removing non-message ID bits) */
            dcmd_data->mid = canmsg->canmid & MID_MASK_EXT;
            ret = (int)sizeof(dcmd_data->mid);
            break;

        case CAN_DEVCTL_SET_MFILTER:
            dcmd_data = _IO_INPUT_PAYLOAD(msg);
            /* Make sure this is a receive mailbox, and we're not in raw mode. */
            if ((mbxid >= devinfo->numrx) ||
                            (devinfo->mode == CANDEV_MODE_RAW_FRAME)) {
                errno = EINVAL;
                return -1;
            }

            /*
             * Rx Individual Mask Registers are used as acceptance masks for ID filtering
             * in Rx message buffers. They can only be accessed by the ARM while the
             * module is in freeze mode. Outside of freeze mode, write accesses are
             * blocked and read accesses return all zeros.
             */
            /* Enable Freeze Mode capability */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_FRZ, MCR_FRZ);
            /* Enter Freeze Mode */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, MCR_HALT);
            /* Wait for indication that FlexCAN in Freeze Mode */
            timeout = 20000;
            while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_FRZACK) != MCR_FRZACK) {
                if (timeout-- == 0) {
                    can_slogf(_SLOG_ERROR, "%s: FlexCAN Freeze Mode failed: FRZ_ACK timeout!");
                    errno = EIO;
                    return -1;
                }
            }

            /* Disable mailbox events */
            if (!(devinfo->flags & FLEXCAN_FLAGS_CANFD)) {
                flexcan_set_imask(devinfo, mbxid, 0);
            } else {
                set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), 0);
            }
            /*
             * Enable local acceptance mask if the filter value is non-zero
             * Set receive filter by programming local acceptance mask (CANLAM).
             * Every bit set in LAM mask means that the corresponding bit
             * in the CANMID is checked
             */
            offset = mbxid * 0x4;
            out32(devinfo->canlam + offset, dcmd_data->mfilter & FLEXCAN_LAM_MASK);

            /* Re-enable mailbox events */
            if (!(devinfo->flags & FLEXCAN_FLAGS_CANFD)) {
                flexcan_set_imask(devinfo, mbxid, 1);
            } else {
                set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), FLEXCAN_INTBIT(mbxid));
            }

            /* Take FlexCAN out of freeze mode */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, 0);

            for (int ntr = 0; ntr < FLEXCAN_SET_MODE_RETRIES; ntr++) {
                cantest = in32(devinfo->base + FLEXCAN_MCR);
                if (!(cantest & (MCR_NOTRDY | MCR_FRZACK))) {
                    break;
                }
            }
            break;

        case CAN_DEVCTL_GET_MFILTER:
            /* Make sure this is a receive mailbox */
            if ((mbxid >= devinfo->numrx)) {
                errno = EINVAL;
                return -1;
            }

            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /*
             * The FLEXCAN must be in Freeze mode for the filter registers to be accessed.
             * Halt FlexCAN and wait for freeze acknowledge (pending TXs and RXs done
             */

            /* Enable Freeze Mode capability */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_FRZ, MCR_FRZ);
            /* Enter Freeze Mode */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, MCR_HALT);

            /* Wait for indication that FlexCAN is in Freeze Mode */
            timeout = 20000;
            while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_FRZACK) != MCR_FRZACK) {
                if (timeout-- == 0) {
                    can_slogf(_SLOG_ERROR, "%s: FlexCAN Freeze Mode failed: FRZ_ACK timeout!", __func__);
                    errno = EIO;
                    return -1;
                }
            }

            offset = mbxid * 0x4;

            /* Read device's current message ID (removing non-message ID bits) */
            dcmd_data->mfilter = in32(devinfo->canlam + offset);

            /* Take FlexCAN out of freeze mode and continue */
            set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, 0);

            for (int ntr = 0; ntr < FLEXCAN_SET_MODE_RETRIES; ntr++) {
                cantest = in32(devinfo->base + FLEXCAN_MCR);
                if (!(cantest & (MCR_NOTRDY | MCR_FRZACK))) {
                    break;
                }
            }
            ret = (int)sizeof(dcmd_data->mfilter);
            break;

        case CAN_DEVCTL_SET_PRIO:
            dcmd_data = _IO_INPUT_PAYLOAD(msg);
            /* Make sure this is a transmit mailbox and priority is valid,
            and we're not in raw mode */
            if ((mbxid < devinfo->numrx) ||
                (dcmd_data->prio > MCF_TPL_MAXVAL) ||
                (devinfo->mode == CANDEV_MODE_RAW_FRAME)) {
                errno = EINVAL;
                return -1;
            }

            if (!(devinfo->flags & FLEXCAN_FLAGS_CANFD)) {
                /* Disable Object */
                flexcan_set_imask(devinfo, mbxid, 0);
                /* Set new priority */
                canmsg->canmcf = (canmsg->canmcf & ~MCF_TPL_MASK) | (dcmd_data->prio << MCF_TPL_SHIFT);
                /* Enable Object */
                flexcan_set_imask(devinfo, mbxid, 1);
            } else {
                /* Disable Object */
                if (mbxid < devinfo->numrx) {
                    set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), 0);
                } else {
                    set_port32(devinfo->base + FLEXCAN_IMASK2, FLEXCAN_INTBIT(mbxid), 0);
                }
                /* Set new priority */
                canmsg->canmcf = (canmsg->canmcf & ~MCF_TPL_MASK) | (dcmd_data->prio << MCF_TPL_SHIFT);
                /* Enable Object */
                if (mbxid < devinfo->numrx) {
                    set_port32(devinfo->base + FLEXCAN_IMASK1, FLEXCAN_INTBIT(mbxid), FLEXCAN_INTBIT(mbxid));
                } else {
                    set_port32(devinfo->base + FLEXCAN_IMASK2, FLEXCAN_INTBIT(mbxid), FLEXCAN_INTBIT(mbxid));
                }
            }
            break;

        case CAN_DEVCTL_GET_PRIO:
            /* Make sure this is a transmit mailbox */
            if (mbxid < devinfo->numrx) {
                errno = EINVAL;
                return -1;
            }

            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);

            /* Read device's TX prio level in CANMCF */
            dcmd_data->prio = (canmsg->canmcf & MCF_TPL_MASK) >> MCF_TPL_SHIFT;
            ret = (int)sizeof(dcmd_data->prio);
            break;

        case CAN_DEVCTL_SET_TIMESTAMP:
            dcmd_data = _IO_INPUT_PAYLOAD(msg);
            /* Check if we should set the Local Network Time or clear the MSB bit.
             We use the max timestamp value since no-one will likely set this value.*/
            if (!(dcmd_data->timestamp == 0xFFFF)) {
                /* Set the current Local Network Time */
                out32(devinfo->base + FLEXCAN_TIMER, dcmd_data->timestamp);
            }
            break;

        case CAN_DEVCTL_GET_TIMESTAMP:
            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /* Read the current Local Network Time */
            dcmd_data->timestamp = in32(devinfo->base + FLEXCAN_TIMER);
            ret = (int)sizeof(dcmd_data->timestamp);
            break;

        case CAN_DEVCTL_ERROR:
            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /* Read current state of CAN Error and Status register */
            dcmd_data->error.drvr1 = in32(devinfo->base + FLEXCAN_ESR);
            /* Read and clear CANES devctl info */
            dcmd_data->error.drvr2 = devinfo->canestat;
            devinfo->canestat = 0;
            /* Read current state of CAN Receive Error Counter Register */
            dcmd_data->error.drvr3 = (in32(devinfo->base + FLEXCAN_ECR) & 0xFF00);
            /* Read current state of CAN Transmit Error Counter Register */
            dcmd_data->error.drvr4 = (in32(devinfo->base + FLEXCAN_ECR) & 0x00FF);
            ret = (int)sizeof(dcmd_data->error);
            break;

        case CAN_DEVCTL_DEBUG_INFO:
            /* Print debug information */
            flexcan_debug(dev);
            break;

        case CAN_DEVCTL_GET_STATS:
            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /* Copy statistics counters to client structure */
            memcpy(&dcmd_data->stats, &devinfo->stats, sizeof(CAN_DEVCTL_STATS));
            ret = (int)sizeof(dcmd_data->stats);
            break;

        case CAN_DEVCTL_GET_INFO:
            dcmd_data = _IO_OUTPUT_PAYLOAD(msg);
            /* Copy statistics counters to client structure */
            strcpy(dcmd_data->info.description, devinfo->initinfo.description);
            dcmd_data->info.msgq_size = devinfo->initinfo.msgq_size;
            dcmd_data->info.waitq_size = devinfo->initinfo.waitq_size;
            dcmd_data->info.mode = devinfo->mode;
            dcmd_data->info.bit_rate = (uint32_t)devinfo->initinfo.bt.bitrate;
            dcmd_data->info.bit_rate_prescaler = (uint16_t)devinfo->initinfo.bt.brp;
            dcmd_data->info.sync_jump_width = (uint8_t)devinfo->initinfo.bt.sjw;
            dcmd_data->info.time_segment_1 = (uint8_t)devinfo->initinfo.bt.phase_seg1;
            dcmd_data->info.time_segment_2 = (uint8_t)devinfo->initinfo.bt.phase_seg2;
            dcmd_data->info.num_tx_mboxes = devinfo->numtx;
            dcmd_data->info.num_rx_mboxes = devinfo->numrx;
            dcmd_data->info.loopback_internal = (devinfo->flags & FLEXCAN_FLAGS_LOOPBACK) ? 1u : 0u;
            dcmd_data->info.autobus_on = (devinfo->flags & FLEXCAN_FLAGS_AUTOBUS) ? 1u : 0u;
            dcmd_data->info.silent = 0;
            ret = (int)sizeof(dcmd_data->info);
            break;

        default:
            /* Driver does not support this DEVCTL */
            errno = ENOTSUP;
            ret = -1;
            break;
    }

    return ret;
}

/**
 *  @brief              Initialize FLEXCAN interrupt
 *  @param  devinfo     flexcan_info_t pointer
 *  @param  devinit     flexcan_init_t pointer
 *
 *  @return             0 --success
 *                      -1 --failure
 */
int  flexcan_init_intr(flexcan_info_t* const devinfo, const flexcan_init_t* const devinit)
{
    /* Attach interrupt handler for ERROR interrupts */
    devinfo->numirq = devinit->numirqs;
    for (int i = 0; i < devinit->numirqs; i++) {
        devinfo->iidsys[i] = can_resmgr_attach_intr(devinfo, devinit->irqsys[i], (short)i);
        if (devinfo->iidsys[i] == -1) {
            for (int j = 0; j < i; j++) {
                can_resmgr_detach_intr(devinfo->iidsys[j], (short)j);
            }
            return -1;
        }
        devinfo->irqsys[i] = devinit->irqsys[i];
    }

    /* Interrupts on Rx, Tx, any Status change and data overrun */
    if (devinfo->mode == CANDEV_MODE_RAW_FRAME) {
        /* Enable raw mailbox interrupts (MB1 to MB2) */
        out32(devinfo->base + FLEXCAN_IMASK1, (RAW_MODE_TX_IRQ | RAW_MODE_RX_IRQ));
    } else {
        out32(devinfo->base + FLEXCAN_IMASK1, 0xFFFFFFFF);
        out32(devinfo->base + FLEXCAN_IMASK2, 0xFFFFFFFF);
        if (!(devinit->flags & FLEXCAN_FLAGS_CANFD) && (devinit->flags & FLEXCAN_FLAGS_DUALMB)) {
            out32(devinfo->base + FLEXCAN_IMASK3, 0xFFFFFFFF);
            out32(devinfo->base + FLEXCAN_IMASK4, 0xFFFFFFFF);
        }
    }

    /* Enable all system/error interrupts to be generated on interrupt line */
    /* Wake Up Interrupt is enabled */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_WAKEMSK, MCR_WAKEMSK);
    /* Bus Off interrupt enabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_BOFFMSK, CTRL_BOFFMSK);
    /* Error interrupt enabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_ERRMASK, CTRL_ERRMASK);
    /* Tx Warning Interrupt enabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_TWRNMSK, CTRL_TWRNMSK);
    /* Rx Warning Interrupt enabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_RWRNMSK, CTRL_RWRNMSK);

    return 0;
}

/**
 *  @brief              Initialize FLEXCAN interface
 *  @param  devinfo     flexcan_info_t pointer
 *  @param  devinit     flexcan_init_t pointer
 *
 *  @return             0 --success
 *                      -1 --failure with errno set
 */
int flexcan_init_hw(flexcan_info_t* const devinfo, const flexcan_init_t* const devinit)
{
    int         timeout = 20000, counter = 0;
    uint32_t    canmcf_ide = 0, cantest = 0;
    uint32_t    canlam = 0;
    can_msg_obj_t volatile *canmsg;

    /* Go to INIT mode:
     * Any configuration change/initialization requires that the FlexCAN be
     * frozen by either asserting the HALT bit in the
     * module configuration register or by reset.
     */

    /* Enable Device */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_MDIS, 0);
    // Wait for indication that FlexCAN not in any of the low power modes
    timeout = 20000;
    while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_LPM_ACK) != 0) {
        if (timeout-- == 0) {
            can_slogf(_SLOG_ERROR, "%s: Not able to come out of Low Power Mode: LPM_ACK timeout!", __func__);
            errno = EIO;
            return -1;
        }
    }

    /* Reset Device */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_SOFTRST, MCR_SOFTRST);
    /* Since soft reset is synchronous and has to follow a request/acknowledge procedure
     * across clock domains, it may take some time to fully propagate its effect.
     */
    timeout = 20000;
    while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_SOFTRST) != 0) {
        if (timeout-- == 0) {
            can_slogf(_SLOG_ERROR, "%s: Enter Init Mode: SOFT_RST timeout!", __func__);
            errno = EIO;
            return -1;
        }
    }
    /* Give reset time */
    nanospin_ns(1000);

    timeout = 20000;
    while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_FRZACK) != MCR_FRZACK) {
        if (timeout-- == 0) {
            can_slogf(_SLOG_ERROR, "%s: Unable to enter Freeze Mode", __func__);
            errno = EIO;
            return -1;
        }
    }

    /* Determine the bit timing parameters: PROPSEG, PSEG1, PSEG2, SJW.
     * Determine the bit rate by programming the PRESDIV field.
     * The prescaler divide register (PRESDIV) allows the user to select
     * the ratio used to derive the S-Clock from the system clock.
     * The time quanta clock operates at the S-clock frequency.
     */
    flexcan_set_bittiming(devinfo, devinit);

    /*
     * For any configuration change/initialization it is required that FlexCAN be put into Freeze Mode.
     * The following is a generic initialization sequence applicable to the FlexCAN module:
     */

    /* 1. Initialize the Module Configuration Register */
    /* Affected registers are in Supervisor memory space */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_SUPV, MCR_SUPV);
    /* Enable the individual filtering per MB and reception queue features by setting the IRMQ bit */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_IRMQ, MCR_IRMQ);
    /* Enable the warning interrupts by setting the WRN_EN bit */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_WRN_EN, MCR_WRN_EN);
    /* If required, disable frame self reception by setting the SRX_DIS bit */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_SRX_DIS, MCR_SRX_DIS);
    /* Enable the abort mechanism by setting the AEN bit */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_AEN, MCR_AEN);
    /* Enable the local priority feature by setting the LPRIO_EN bit */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_LPRIO_EN, MCR_LPRIO_EN);
    /* Format A One full ID (standard or extended) per filter element */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_IDAM_FormatA, MCR_IDAM_FormatA);
    /* Maximum MBs in use */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_MAXMB_MASK, MCR_MAXMB_MAXMB(devinfo->numrx + devinfo->numtx - 1));

    /* 2. Initialize the Control Register */
    /* Disable Bus-Off Interrupt */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_BOFFMSK, 0);
    /* Disable Error Interrupt */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_ERRMASK, 0);
    /* Tx Warning Interrupt disabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_TWRNMSK, 0);
    /* Rx Warning Interrupt disabled */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_RWRNMSK, 0);

    /* Rx mailbox IDE and RTR are compared to filter */
    if (devinfo->mode == CANDEV_MODE_RAW_FRAME) {
        set_port32(devinfo->base + FLEXCAN_CTRL2, CTRL2_EACEN, CTRL2_EACEN);
        canlam = 0;
    } else {
        canlam = 0xffffffff;
    }

    /* Single Sample Mode should be set by default */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_SAMP, 0);
    /* Check if Triple Sample Mode should be set */
    if ((devinit->flags & FLEXCAN_FLAGS_BITRATE_SAM) && (devinit->bt.brp >= CTRL_PRESDIV_SAM_MIN)) {
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_SAMP, CTRL_SAMP);
    }
    /* Disable self-test/loop-back by default */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LPB, 0);
    /* Enable self-test/loop-back for testing */
    if (devinit->flags & FLEXCAN_FLAGS_LOOPBACK) {
        set_port32(devinfo->base + FLEXCAN_MCR, MCR_SRX_DIS, 0);
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LPB, CTRL_LPB);
    }
    /* Disable Timer Sync feature by default */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_TSYNC, 0);
    /* Enable Timer Sync feature */
    if (devinit->flags & FLEXCAN_FLAGS_TSYN) {
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_TSYNC, CTRL_TSYNC);
    }

    /* Listen-Only Mode:
     * In listen-only mode, the CAN module is able to receive messages
     * without giving an acknowledgment.
     * Since the module does not influence the CAN bus in this mode
     * the host device is capable of functioning like a monitor
     * or for automatic bit-rate detection.
     */
    /* De-activate Listen Only Mode by default */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LOM, 0);
    /* Select Bus monitor mode if flag set */
    if (devinit->flags & FLEXCAN_FLAGS_LOM) {
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LOM, CTRL_LOM);
    }
    /* Enable Automatic recovering from Bus Off state */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_BOFFREC, 0);
    /* Disable Auto bus on */
    if (devinit->flags & FLEXCAN_FLAGS_AUTOBUS) {
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_BOFFREC, CTRL_BOFFREC);
    }

    /* Determine the internal arbitration mode (LBUF bit) */
    /* The LBUF bit defines the transmit-first scheme.
     * 0 = Message buffer with lowest ID is transmitted first.
     * 1 = Lowest numbered buffer is transmitted first.
     */
    /* Buffer with highest priority is transmitted first */
    set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LBUF, 0);
    /* Lowest number buffer is transmitted first */
    if (devinit->flags & FLEXCAN_FLAGS_LBUF) {
        set_port32(devinfo->base + FLEXCAN_CTRL, CTRL_LBUF, CTRL_LBUF);
    }

    if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
        uint32_t    reg_fdctrl = 0, reg_mcr = 0, reg_ctrl2 = 0;

        /* FDCTRL
        * support BRS when set CAN FD mode
        * 64 bytes payload per MB and 7 MBs per RAM block by default
        * enable CAN FD mode
        */
        reg_fdctrl = in32(devinfo->base + FLEXCAN_FDCTRL);
        reg_fdctrl |= FDCTRL_FDRATE;
        reg_fdctrl |= FDCTRL_MBDSR3(3) | FDCTRL_MBDSR2(3) | FDCTRL_MBDSR1(3) | FDCTRL_MBDSR0(3);
        if (devinit->flags & FLEXCAN_FLAGS_TDCEN) {
            reg_fdctrl &= ~FDCTRL_TDCOFF(0x1f);
            reg_fdctrl |= FDCTRL_TDCEN | FDCTRL_TDCOFF(devinit->tdcoff);
        }

        out32(devinfo->base + FLEXCAN_FDCTRL, reg_fdctrl);

        reg_mcr = in32(devinfo->base + FLEXCAN_MCR);
        reg_mcr |= MCR_FDEN;
        out32(devinfo->base + FLEXCAN_MCR, reg_mcr);

        reg_ctrl2 = in32(devinfo->base + FLEXCAN_CTRL2);
        if (devinit->flags & FLEXCAN_FLAGS_ISO) {
            reg_ctrl2 |= CTRL2_ISOCANFDEN;
        } else {
            reg_ctrl2 &= ~(CTRL2_ISOCANFDEN);
        }
        out32(devinfo->base + FLEXCAN_CTRL2, reg_ctrl2);
    }

    /* Set initial value for Local Network Time */
    if (devinit->flags & FLEXCAN_FLAGS_TIMESTAMP) {
        out32(devinfo->base + FLEXCAN_TIMER, devinit->timestamp);
    }

    /* Clear interrupts, error counters and set mask registers */

    /* Global registers instead of individual regsiters (Negate IRMQ) */
    if ((in32(devinfo->base + FLEXCAN_MCR) & MCR_IRMQ) == 0) {
        out32(devinfo->base + FLEXCAN_RXGMASK, 0xffffffffu);
        out32(devinfo->base + FLEXCAN_RX14MASK, 0xffffffffu);
        out32(devinfo->base + FLEXCAN_RX15MASK, 0xffffffffu);
    }
    out32(devinfo->base + FLEXCAN_ECR, 0);
    out32(devinfo->base + FLEXCAN_ESR, in32(devinfo->base + FLEXCAN_ESR) | 0xffffffffu);
    /* Disable Buffer Interrupt */
    out32(devinfo->base + FLEXCAN_IMASK2, 0);
    out32(devinfo->base + FLEXCAN_IMASK1, 0);
    /* Clear Interrupt Flag by writing it to 1 */
    out32(devinfo->base + FLEXCAN_IFLAG2, 0xffffffffu);
    out32(devinfo->base + FLEXCAN_IFLAG1, 0xffffffffu);
    if (!(devinit->flags & FLEXCAN_FLAGS_CANFD) && (devinit->flags & FLEXCAN_FLAGS_DUALMB)) {
        out32(devinfo->base + FLEXCAN_IMASK4, 0);
        out32(devinfo->base + FLEXCAN_IMASK3, 0);
        out32(devinfo->base + FLEXCAN_IFLAG4, 0xffffffffu);
        out32(devinfo->base + FLEXCAN_IFLAG3, 0xffffffffu);
    }

    /*
     * Initialize CAN mailboxes in device memory
     */

    /* Check if the 29 bit extended identifier should be enabled */
    if (devinit->flags & FLEXCAN_FLAGS_EXTENDED_MID) {
        canmcf_ide |= MCF_IDE;
    }

    /* Configure Receive Mailboxes */
    counter = 0;

    for (uint32_t mbx = 0; mbx < devinfo->numrx; mbx++) {
        /* Disable mailbox to configure message object
         * The control/status word of all message buffers are written
         * as an inactive receive message buffer.
         */
        canmsg = flexcan_get_mb(devinfo, mbx);
        if (canmsg == NULL) {
            can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
            errno = EINVAL;
            return -1;
        }

        canmsg->canmcf = ((REC_CODE_NOT_ACTIVE & 0x0F) << 24);
        /* Initialize default receive message ID */
        if (canmcf_ide) {   /* Extended frame */
            canmsg->canmid = (uint32_t)((devinit->midrx + FLEXCAN_MID_DEFAULT * counter++) & MID_MASK_EXT);
        } else {    /* Standard frame */
            canmsg->canmid = (uint32_t)((devinit->midrx + FLEXCAN_MID_DEFAULT * counter++) & MID_MASK_STD);

        }

        /* Put MB into rx queue */
        canmsg->canmcf = ((REC_CODE_EMPTY & 0x0F) << 24) | canmcf_ide;
    }

    /* Configure Transmit Mailboxes */
    counter = 0;
    for (uint32_t mbx = devinfo->numrx; mbx < (devinfo->numrx + devinfo->numtx); mbx++) {
        /* Disable mailbox to configure message object
         Trasmission inactive, Set message data size */
        canmsg = flexcan_get_mb(devinfo, mbx);
        if (canmsg == NULL) {
            can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
            errno = EINVAL;
            return -1;
        }

        canmsg->canmcf = ((TRANS_CODE_NOT_READY & 0x0F) << 24) | (CAN_MAX_DLC << 16) | canmcf_ide;
        /* Initialize default transmit message ID */
        if (canmcf_ide) {   /* Extended frame */
            canmsg->canmid = (devinit->midtx + FLEXCAN_MID_DEFAULT * counter++) & MID_MASK_EXT;
        } else {    /* Standard frame */
            canmsg->canmid = (devinit->midtx + FLEXCAN_MID_DEFAULT * counter++) & MID_MASK_STD;
        }
    }

    /*
     * Follow the protocol as described in "Detection and Correction of
     * Memory Errors" to write to MECR register
     */
    set_port32(devinfo->base + FLEXCAN_CTRL2, CTRL2_ECRWRE, CTRL2_ECRWRE);
    if ((devinfo->flags & FLEXCAN_FLAGS_CANFD) || (devinfo->flags & FLEXCAN_FLAGS_DISABLE_MECR)) {
        set_port32(devinfo->base + FLEXCAN_MECR, MECR_ECRWRDIS, 0);
        set_port32(devinfo->base + FLEXCAN_MECR, MECR_NCEFAFRZ, 0);
        set_port32(devinfo->base + FLEXCAN_MECR, MECR_HANCEI_MSK, 0);
        set_port32(devinfo->base + FLEXCAN_MECR, MECR_FANCEI_MSK, 0);
        set_port32(devinfo->base + FLEXCAN_MECR, MECR_ECRWRDIS, 1);
    }

    /* Enable the FlexCAN module */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_MDIS, 0);
    /* Wait for indication that FlexCAN not in any of the low power modes */
    timeout = 20000;
    while ((in32(devinfo->base + FLEXCAN_MCR) & MCR_LPM_ACK) != 0) {
        if (timeout-- == 0) {
            can_slogf(_SLOG_ERROR, "%s: Not able to come out of Low Power Mode: LPM_ACK timeout!", __func__);
            errno = EIO;
            return -1;
        }
    }

    /* Rx Individual Mask Registers (RXIMR0-RXIMR63):
     * 1: The corresponding bit in the filter is checked against the one received
     * 0: the corresponding bit in the filter is don't care
     */
    uint32_t    rmbi = 0;
    while (rmbi <= (FLEXCAN_MAILBOX_SIZE * devinfo->numrx)) {
        out32(devinfo->canlam + rmbi, canlam);
        rmbi += FLEXCAN_MAILBOX_SIZE;
    }

    /* Synchronize with can bus */
    set_port32(devinfo->base + FLEXCAN_MCR, MCR_HALT, 0);

    for (int i = 0; i < FLEXCAN_SET_MODE_RETRIES; i++) {
        cantest = in32(devinfo->base + FLEXCAN_MCR);
        if (!(cantest & (MCR_NOTRDY | MCR_FRZACK))) break;
    }

    cantest = in32(devinfo->base + FLEXCAN_MCR);
    if (!(cantest & MCR_FRZACK)) {
        can_slogf(_SLOG_INFO, "%s: Out from Freezemode", __func__);
    } else {
        can_slogf(_SLOG_WARNING, "%s: Can't get out from Freezemode", __func__);
    }

    return 0;
}

/**
 *  @brief              Transmit a CAN message from the specified mailbox
 *  @param  dev         FLEXCAN device pointer
 *  @param  txmsg       CAN message to transmit
 *
 *  @return             None
 */
static void flexcan_tx(flexcan_t* const cdev, canmsg_t* const txmsg)
{
    flexcan_info_t* const devinfo = cdev->devinfo;
    const uint32_t  mbxid = cdev->mbxid;
    /* Access the data as a uint32_t array */
    uint32_t        can_mcf;
    canfd_msg_obj_t *canmsg;

    /* clear the IFLAG bit */
    flexcan_clear_iflag(devinfo, mbxid);

    canmsg = (canfd_msg_obj_t *)flexcan_get_mb(devinfo, mbxid);
    if (canmsg == NULL) {
        can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbxid);
        return;
    }

    /* check if transmission is active */
    can_mcf = canmsg->canmcf;
    if ((((can_mcf >> 24) & 0x0F) != TRANS_CODE_NOT_READY) &&
            (((can_mcf >> 24) & 0x0F) != TRANS_CODE_ABORT)) {
        /* disable the related interrupt */
        flexcan_set_imask(devinfo, mbxid, 0);

        /* transmission is active so write ABORT to the CODE field */
        canmsg->canmcf |= ((TRANS_CODE_ABORT & 0x0F) << 24);

        /* wait for the corresponding IFLAG bit */
        while (flexcan_get_iflag(devinfo, mbxid)) ;

        /* clear the IFLAG bit */
        flexcan_clear_iflag(devinfo, mbxid);

        /* enable the interrupt again */
        flexcan_set_imask(devinfo, mbxid, 1);
    }

    /*
     * Set mailbox to inactive while filling it.
     * Also set the msg length in the mcf to the actual message length
     */
    can_mcf = canmsg->canmcf;
    can_mcf &= ~(MSG_BUF_CODE_MASK | MSG_BUF_DLC_MASK);
    can_mcf |= TRANS_CODE_NOT_READY << MSG_BUF_CODE_SHIFT;

    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        can_mcf |= MB_CNT_EDL;
        can_mcf |= MB_CNT_BRS;
    }
    can_mcf |= flexcan_len2dlc(txmsg->cmsg.len) << MSG_BUF_DLC_SHIFT;

    /*
     * if we are in raw mode, set the MCF_IDE bit if txmsg is an extended message
     * Otherwise just use the bit set when the mailbox was configured i
     */
    if (devinfo->mode == CANDEV_MODE_RAW_FRAME) {
        can_mcf &= ~MCF_IDE;
        can_mcf |= (txmsg->cmsg.ext.is_extended_mid) ? MCF_IDE : 0;
    }

    canmsg->canmcf = can_mcf;

    /* Copy message data into transmit mailbox */
    uint32_t* src32 = (uint32_t *)txmsg->cmsg.dat;
    uint32_t* const dst32 = (uint32_t *)&canmsg->canmd[0];
    int wlen = CAN_MAX_DLEN / 4;
    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        wlen *= (CANFD_MAX_DLEN / CAN_MAX_DLEN);
    }
    for (int widx = 0; widx < wlen; widx += 1) {
        if (devinfo->flags & FLEXCAN_FLAGS_ENDIAN_SWAP) {
            ENDIAN_SWAP32(&src32[widx]);
        }
        dst32[widx] = src32[widx];
    }

    /* Transmission active */
    canmsg->canmcf |= ((TRANS_CODE_TRANSMIT_ONCE & 0x0F) << 24);
}

/**
 *  @brief              Print CAN device registers
 *  @param  devinfo     flexcan_info_t pointer
 *
 *  @return             None
 */
static void flexcan_print_reg(flexcan_info_t* const devinfo)
{
    fprintf(stderr, "\n******************************************************\n");
    fprintf(stderr, "CANMCR = 0x%02x\t", in32(devinfo->base + FLEXCAN_MCR));
    fprintf(stderr, "   CANCTRL = 0x%02x\t", in32(devinfo->base + FLEXCAN_CTRL));
    fprintf(stderr, "   CANCTRL2 = 0x%02x\n", in32(devinfo->base + FLEXCAN_CTRL2));
    fprintf(stderr, "CANTIMER = 0x%02x\t", in32(devinfo->base + FLEXCAN_TIMER));
    fprintf(stderr, "   CANECR = 0x%02x\t", in32(devinfo->base + FLEXCAN_ECR));
    fprintf(stderr, "   CANESR = 0x%02x\n", in32(devinfo->base + FLEXCAN_ESR));
    fprintf(stderr, "CANIMASK2 = 0x%02x\t", in32(devinfo->base + FLEXCAN_IMASK2));
    fprintf(stderr, "   CANIMASK1 = 0x%02x\n", in32(devinfo->base + FLEXCAN_IMASK1));
    fprintf(stderr, "CANIFLAG2  = 0x%02x\t", in32(devinfo->base + FLEXCAN_IFLAG2));
    fprintf(stderr, "   CANIFLAG1 = 0x%02x\n", in32(devinfo->base + FLEXCAN_IFLAG1));

    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        fprintf(stderr, "CANIMASK4 = 0x%02x\t", in32(devinfo->base + FLEXCAN_IMASK4));
        fprintf(stderr, "   CANIMASK3 = 0x%02x\n", in32(devinfo->base + FLEXCAN_IMASK3));
        fprintf(stderr, "CANIFLAG4  = 0x%02x\t", in32(devinfo->base + FLEXCAN_IFLAG4));
        fprintf(stderr, "   CANIFLAG3 = 0x%02x\n", in32(devinfo->base + FLEXCAN_IFLAG3));
    }

    if (devinfo->flags & FLEXCAN_FLAGS_DUALMB) {
        fprintf(stderr, "CBT = 0x%02x\t", in32(devinfo->base + FLEXCAN_CBT));
        fprintf(stderr, "FDCTRL = 0x%02x\t", in32(devinfo->base + FLEXCAN_FDCTRL));
        fprintf(stderr, "   FDCBT = 0x%02x\n", in32(devinfo->base + FLEXCAN_FDCBT));
    }
    fprintf(stderr, "******************************************************\n");
}

/**
 *  @brief              Print CAN device mailbox memory
 *  @param  dev         FLEXCAN device pointer
 *
 *  @return             None
 */
static void flexcan_print_mailbox(flexcan_info_t* const devinfo)
{
    fprintf(stderr, "RX Mailboxes\n");
    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        fprintf(stderr, "DEV\tMBX\tMID\t\tMCF\t\n");
    } else {
        fprintf(stderr, "DEV\tMBX\tMID\t\tMCF\t\tMDH\t\tMDL\n");
    }
    fprintf(stderr, "==========================================================================\n");
    for (uint32_t mbx = 0; mbx < devinfo->num_mailboxes; mbx++) {
        if (devinfo->devlist[mbx].cdev.devtype == CANDEV_TYPE_RX) {
            const canfd_msg_obj_t* const canfdmsg = (canfd_msg_obj_t *)flexcan_get_mb(devinfo, mbx);
            if (canfdmsg == NULL) {
                can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
                return;
            }
            if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
                if (mbx != 0) {
                    fprintf(stderr, "\nrx%d\t%02d\t0x%08X\t0x%08X\t\n",
                            devinfo->devlist[mbx].cdev.dev_unit, mbx, canfdmsg->canmid, canfdmsg->canmcf);
                    for (int nd = 0; nd < CANFD_MAX_DLEN; nd++) {
                        fprintf(stderr, "%.2x    ", canfdmsg->canmd[nd]);
                        if ((nd + 1) % 16 == 0) {
                            fprintf(stderr, "\n");
                        }
                    }
                }
            } else {
                const can_msg_obj_t* const canmsg = flexcan_get_mb(devinfo, mbx);
                if (canmsg == NULL) {
                    can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
                    return;
                }
                fprintf(stderr, "rx%d\t%02d\t0x%08X\t0x%08X\t0x%08X\t0x%08X\t\n",
                        devinfo->devlist[mbx].cdev.dev_unit, mbx,
                        canmsg->canmid, canmsg->canmcf, canmsg->canmdh, canmsg->canmdl);
            }
        }
    }

    fprintf(stderr, "\nTX Mailboxes\n");
    if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
        fprintf(stderr, "DEV\tMBX\tMID\t\tMCF\t\n");
    } else {
        fprintf(stderr, "DEV\tMBX\tMID\t\tMCF\t\tMDH\t\tMDL\n");
    }

    fprintf(stderr, "==========================================================================\n");
    for (uint32_t mbx = 0; mbx < devinfo->num_mailboxes; mbx++) {
        if (devinfo->devlist[mbx].cdev.devtype == CANDEV_TYPE_TX) {
            const canfd_msg_obj_t* const canfdmsg = (canfd_msg_obj_t *)flexcan_get_mb(devinfo, mbx);
            if (canfdmsg == NULL) {
                can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
                return;
            }
            if (devinfo->flags & FLEXCAN_FLAGS_CANFD) {
                if (mbx != devinfo->numrx) {
                    fprintf(stderr, "\ntx%d\t%02d\t0x%08X\t0x%08X\t\n",
                        devinfo->devlist[mbx].cdev.dev_unit, mbx, canfdmsg->canmid, canfdmsg->canmcf);
                    for (int nd = 0; nd < CANFD_MAX_DLEN; nd++) {
                        fprintf(stderr, "%.2x    ", canfdmsg->canmd[nd]);
                        if ((nd + 1) % 16 == 0) {
                            fprintf(stderr, "\n");
                        }
                    }
                }
            } else {
                const can_msg_obj_t* const canmsg = flexcan_get_mb(devinfo, mbx);
                if (canmsg == NULL) {
                    can_slogf(_SLOG_ERROR, "%s: mailbox %d doesn't exist!", __func__, mbx);
                    return;
                }
                fprintf(stderr, "tx%d\t%02d\t0x%08X\t0x%08X\t0x%08X\t0x%08X\t\n",
                        devinfo->devlist[mbx].cdev.dev_unit, mbx,
                        canmsg->canmid, canmsg->canmcf, canmsg->canmdh, canmsg->canmdl);
            }
        }
    }

    fprintf(stderr, "==========================================================================\n");
}

/**
 *  @brief              Print out debug information
 *  @param  dev         FLEXCAN device pointer
 *
 *  @return             None
 */
static void flexcan_debug(flexcan_t* const dev)
{
    fprintf(stderr, "\nCAN REG\n");
    flexcan_print_reg(dev->devinfo);
    fprintf(stderr, "\nMailboxes\n");
    flexcan_print_mailbox(dev->devinfo);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/can/flexcan/flexcan.c $ $Rev: 985670 $")
#endif
