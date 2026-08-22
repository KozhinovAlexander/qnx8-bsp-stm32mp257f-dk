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

#include <drvr/hwinfo.h>

#include "flexcan.h"


static const struct flexcan_btm_const flexcan_bittiming_const = {
    .tseg1_min  = 4,
    .tseg1_max  = 16,
    .tseg2_min  = 2,
    .tseg2_max  = 8,
    .sjw_max    = 4,
    .brp_min    = 1,
    .brp_max    = 256,
    .brp_inc    = 1,
};

static const struct flexcan_btm_const flexcan_fd_bittiming_const = {
    .tseg1_min  = 2,
    .tseg1_max  = 64,
    .tseg2_min  = 1,
    .tseg2_max  = 32,
    .sjw_max    = 32,
    .brp_min    = 1,
    .brp_max    = 1024,
    .brp_inc    = 1,
};

static const struct flexcan_btm_const flexcan_fd_data_bittiming_const = {
    .tseg1_min  = 1,
    .tseg1_max  = 39,
    .tseg2_min  = 1,
    .tseg2_max  = 8,
    .sjw_max    = 8,
    .brp_min    = 1,
    .brp_max    = 1024,
    .brp_inc    = 1,
};

/* Local function prototypes */
static flexcan_info_t* flexcan_device_init(const int argc, char *argv[]);
static flexcan_info_t* flexcan_create_device(flexcan_init_t* const devinit);
static int             flexcan_get_hwinfo(flexcan_hwinfo_t* const canhwinfo, const int unit);
static void            flexcan_fini(flexcan_info_t* const devinfo);

/**
 *  @brief             Main function.
 *  @param argc        Arguments counter
 *  @param argv        Arguments string array.
 *
 *  @return            EXIT_SUCCESS on success; EXIT_FAILURE otherwise
 */
int main(int argc, char *argv[])
{
    /* Process options and create devices */
    flexcan_info_t* const devinfo = flexcan_device_init(argc, argv);
    if (devinfo == NULL) return EXIT_FAILURE;

    /* Start Handling Clients */
    can_resmgr_start();

    flexcan_fini(devinfo);

    return EXIT_SUCCESS;
}

/**
 *  @brief              FLEXCAN driver cleanup
 *  @param  devinfo     Flexcan device handle
 *
 *  @return             None
 */
static void flexcan_fini(flexcan_info_t* const devinfo)
{
    if (devinfo != NULL) {
        if (devinfo->devlist != NULL) {
            for (uint32_t i = 0; i < devinfo->num_mailboxes; i++) {
                can_resmgr_destroy_device(&devinfo->devlist[i].cdev);
            }

            for (int i = 0; i < devinfo->numirq; i++) {
                if (devinfo->irqsys[i] == -1) break;
                can_resmgr_detach_intr(devinfo->iidsys[i], (short)i);
            }

            free(devinfo->devlist);
        }

        if (devinfo->base != (uintptr_t)MAP_FAILED) {
            munmap((void *)devinfo->base, FLEXCAN_SIZE);
        }

        free(devinfo);
    }

    can_resmgr_fini();
}

/**
 *  @brief              Get FLEXCAN base address, IRQ from system page
 *  @param  candev      Pointer to FLEXCAN device hardware info structure
 *  @param  unit        CAN unit number
 *
 *  @return             0 --success
 *                      -1 --failure with errno set
 */
static int flexcan_get_hwinfo(flexcan_hwinfo_t* const canhwinfo, const int unit)
{
    unsigned hwi_off;

    hwi_off = hwi_find_bus(HWI_ITEM_BUS_CAN, (unsigned)unit);
    if (hwi_off == HWI_NULL_OFF) {
        errno = ENODEV;
        return -1;
    }

    hwiattr_can_t attr;
    hwiattr_get_can(hwi_off, &attr);
    canhwinfo->regbase = attr.common.location.base;
    if (attr.common.num_irq > 0) {
        unsigned irq = 0;
        unsigned vector;
        for (;;) {
            vector = hwitag_find_ivec(hwi_off, &irq);
            if (vector == HWI_ILLEGAL_VECTOR) break;
            canhwinfo->irqvector[irq - 1] = (int)vector;
        }
        canhwinfo->numirq = (int)attr.common.num_irq;
    }

    return 0;
}


/**
 *  @brief              Initialize FLEXCAN interface
 *  @param  argc        Argument count
 *  @param  argv        Argument list
 *
 *  @return             Pointer to flexcan_info_t  --success
 *                      NULL --failure
 */
static flexcan_info_t* flexcan_device_init(const int argc, char *argv[])
{
    int         opt;
    int         uid = 0;
    int         found = 0;
    char        *cp;
    /* Set default options */
    flexcan_init_t   devinit = {
        /* FlexCAN */
        .cinit = {
            .devtype = CANDEV_TYPE_RX,  /* devtype */
            .can_unit = -1,             /* can_unit - set this later */
            .dev_unit = 0,              /* dev_unit - set this later*/
            .msgq_size = 128,           /* msgq_size  - max number of queued CAN messages per mailbox */
            .waitq_size = 16,           /* waitq_size - Length of CAN message data  */
            .mode = CANDEV_MODE_IO      /* mode -  CAN driver mode - I/O or raw frames */
        },
        .port = -1,                     /* port */
        .bt = {
            .freq = FLEXCAN_CLK_PLL,    /* bt.freq */
            .bitrate = CAN_BITRATE_50K, /* bt.bitrate */
            .sample_point = 0,          /* bt.sample_point */
            .tq = 0,                    /* bt.tq */
            .prop_seg = 0,              /* bt.prop_seg */
            .phase_seg1 = 0,            /* bt.phase_seg1 */
            .phase_seg2 = 0,            /* bt.phase_seg2 */
            .sjw = 0,                   /* bt.sjw */
            .brp = 0                    /* bt.brp */
        },
        .dbt = {
            .freq = FLEXCAN_CLK_PLL,    /* dbt.freq */
            .bitrate = CAN_BITRATE_50K, /* dbt.bitrate */
            .sample_point = 0,          /* dbt.sample_point */
            .tq = 0,                    /* dbt.tq */
            .prop_seg = 0,              /* dbt.prop_seg */
            .phase_seg1 = 0,            /* dbt.phase_seg1 */
            .phase_seg2 = 0,            /* dbt.phase_seg2 */
            .sjw = 0,                   /* dbt.sjw */
            .brp = 0                    /* dbt.brp */
        },
        .irqsys = { [0] = -1, [1] = -1, [2] = -1 },       /* irqsys */
        .numirqs = 0,
        .flags = FLEXCAN_FLAGS_DUALMB |
                 FLEXCAN_FLAGS_IOPRIV,
        .numtx = 0,                     /* numtx - number of transmit mailboxes */
        .numrx = 0,                     /* numrx - number of receive mailboxes */
        .midrx = 0x100C0000,            /* midrx */
        .midtx = 0x100C0000,            /* midtx */
        .timestamp = 0x0,               /* timestamp */
        .verbosity = _SLOG_ERROR,
        .tdcoff = 0                     /* TDC offset */
    };

    /* Process command line options and create associated devices */
    while (optind < argc) {
        /* Process dash options */
        while ((opt = getopt(argc, argv, "ab:B:c:d:D:fhi:Ilm:Mn:op:q:r:RsStu:vwxz")) != -1) {
            switch (opt) {
            case 'a':
                devinit.flags |= FLEXCAN_FLAGS_AUTOBUS;
                break;

            case 'b': {
                    if (strncmp(optarg, "50K", 3) == 0) {
                        devinit.bt.bitrate = CAN_BITRATE_50K;
                    } else if (strncmp(optarg, "125K", 4) == 0) {
                        devinit.bt.bitrate = CAN_BITRATE_125K;
                    } else if (strncmp(optarg, "250K", 4) == 0) {
                        devinit.bt.bitrate = CAN_BITRATE_250K;
                    } else if (strncmp(optarg, "500K", 4) == 0) {
                        devinit.bt.bitrate = CAN_BITRATE_500K;
                    } else if (strncmp(optarg, "1M", 2) == 0) {
                        devinit.bt.bitrate = CAN_BITRATE_1000K;
                    } else {
                        can_slogf(_SLOG_ERROR,
                                "%s: Unrecognized bitrate value passed in -b option[%s]", __func__, optarg);
                        return NULL;
                    }
                    devinit.bt.sample_point = 0;
                }
                break;

            case 'B':
                /* Values to program bitrate manually */
                devinit.bt.brp = (int)strtoul(optarg, &cp, 0);

                if ((cp != NULL) && (*cp == ',')) {
                    devinit.bt.prop_seg = (int)strtoul(cp + 1, &cp, 0);
                    if ((cp != NULL) && (*cp ==',')) {
                        devinit.bt.phase_seg1 = (int)strtoul(cp + 1, &cp, 0);
                        if ((cp != NULL) && (*cp == ',')) {
                            devinit.bt.phase_seg2 = (int)strtoul(cp + 1, &cp, 0);
                            if ((cp != NULL) && (*cp == ',')) {
                                devinit.bt.sjw = (int)strtoul(cp + 1, NULL, 0);
                            }
                        }
                    }
                }

                /* Need to check for valid bitrate settings */
                devinit.flags |= FLEXCAN_FLAGS_MTIMING;
                break;

            case 'c':
                devinit.flags |= FLEXCAN_FLAGS_TDCEN;
                devinit.tdcoff = (uint8_t)strtoul(optarg, NULL, 0);
                break;

            case 'd': {
                    if (strncmp(optarg, "50K", 3) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_50K;
                    } else if (strncmp(optarg, "125K", 4) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_125K;
                    } else if (strncmp(optarg, "250K", 4) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_250K;
                    } else if (strncmp(optarg, "500K", 4) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_500K;
                    } else if (strncmp(optarg, "1M", 2) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_1000K;
                    } else if (strncmp(optarg, "5M", 2) == 0) {
                        devinit.dbt.bitrate = CAN_BITRATE_5000K;
                    } else {
                        can_slogf(_SLOG_ERROR,
                                "%s: Unrecognized data bitrate value passed in -d option[%s]", __func__, optarg);
                        return NULL;
                    }
                    devinit.dbt.sample_point = 0;
                }
                break;

            case 'D':
                /* Values to program bitrate manually */
                devinit.dbt.brp = (int)strtoul(optarg, &cp, 0);

                if ((cp != NULL) && (*cp == ',')) {
                    devinit.dbt.prop_seg = (int)strtoul(cp + 1, &cp, 0);
                    if ((cp != NULL) && (*cp == ',')) {
                        devinit.dbt.phase_seg1 = (int)strtoul(cp + 1, &cp, 0);
                        if ((cp != NULL) && (*cp ==',')) {
                            devinit.dbt.phase_seg2 = (int)strtoul(cp + 1, &cp, 0);
                            if ((cp != NULL) && (*cp == ',')) {
                                devinit.dbt.sjw = (int)strtoul(cp + 1, &cp, 0);
                            }
                        }
                    }
                }

                /* Need to check for valid bitrate settings */
                devinit.flags |= FLEXCAN_FLAGS_MDTIMING;
                break;

            case 'f':
                devinit.flags |= FLEXCAN_FLAGS_CANFD;
                devinit.cinit.msgq_size  = 28;
                devinit.cinit.waitq_size = 72;
                break;

            case 'h':
                devinit.flags &= ~FLEXCAN_FLAGS_DUALMB;
                break;

            case 'i':
                devinit.midrx = (uint32_t)strtoul(optarg, &cp, 16);
                if ((cp != NULL) && (*cp == ',')) {
                    devinit.midtx = (uint32_t)strtoul(cp + 1, NULL, 0);
                }
                break;

            case 'I':
                devinit.flags |= FLEXCAN_FLAGS_IOPRIV;
                break;

            case 'l':
                devinit.flags = FLEXCAN_FLAGS_LOM;
                break;

            case 'm':
                devinit.flags |= FLEXCAN_FLAGS_TIMESTAMP;
                devinit.timestamp = (uint32_t)strtoul(optarg, NULL, 16);
                break;

            case 'M':
                devinit.flags |= FLEXCAN_FLAGS_DISABLE_MECR;
                break;

            case 'n':
                devinit.cinit.msgq_size = (uint32_t)strtoul(optarg, NULL, 0);
                break;

            case 'o':
                devinit.flags |= FLEXCAN_FLAGS_ISO;
                break;

            case 'p':
                devinit.port = strtoul(optarg, &optarg, 0);
                break;

            case 'q':
                devinit.cinit.waitq_size = (uint32_t)strtoul(optarg, NULL, 0);
                break;

            case 'r':
                cp = optarg;
                while (cp != NULL) {
                    if (devinit.numirqs >= FLEXCAN_MAX_IRQ) {
                        can_slogf(_SLOG_ERROR, "%s: Too many IRQs", __func__);
                        return NULL;
                    }
                    devinit.irqsys[devinit.numirqs++] = (int)strtoul(cp, &cp, 0);
                    if ((cp == NULL) || (*cp != ',')) break;
                    cp++;
                }
                break;

            case 'R':
                devinit.cinit.mode = CANDEV_MODE_RAW_FRAME;
                devinit.numrx = RAW_MODE_RX_NUM_MBOX;
                devinit.numtx = RAW_MODE_TX_NUM_MBOX;
                break;

            case 's':
                devinit.flags |= FLEXCAN_FLAGS_BITRATE_SAM;
                break;

            case 'S':
                devinit.flags |= FLEXCAN_FLAGS_ENDIAN_SWAP;
                break;

            case 't':
                devinit.flags |= FLEXCAN_FLAGS_LOOPBACK;
                break;

            case 'u':
                devinit.cinit.can_unit = (int)strtoul(optarg, NULL, 0);
                break;

            case 'v':
                devinit.verbosity++;
                break;

            case 'w':
                devinit.flags |= FLEXCAN_FLAGS_LBUF;
                break;

            case 'x':
                devinit.flags |= FLEXCAN_FLAGS_EXTENDED_MID;
                break;

            case 'z':
                devinit.flags |= FLEXCAN_FLAGS_TSYN;
                break;

            default:
                break;
            }
        }

        /* Need to calculate or validate bit timing */
        if (devinit.flags & FLEXCAN_FLAGS_CANFD) {
            if (devinit.flags & FLEXCAN_FLAGS_MTIMING) {
                /* Manual bit timing */
                if (flexcan_validate_bittime_params(&flexcan_fd_bittiming_const, &devinit.bt) != 0) {
                    can_slogf(_SLOG_ERROR, "%s: Invalid manual bitrate settings", __func__);
                    return NULL;
                }
            } else {
                flexcan_calc_bittiming(&flexcan_fd_bittiming_const, &devinit.bt);
            }
            if (devinit.flags & FLEXCAN_FLAGS_MDTIMING) {
                /* Manual data bit timing */
                if (flexcan_validate_bittime_params(&flexcan_fd_data_bittiming_const, &devinit.dbt) != 0) {
                    can_slogf(_SLOG_ERROR, "%s: Invalid manual FD bitrate settings", __func__);
                    return NULL;
                }
            } else {
                /* Calculate bit timing */
                flexcan_calc_bittiming(&flexcan_fd_data_bittiming_const, &devinit.dbt);
            }
        } else {
            if (devinit.flags & FLEXCAN_FLAGS_MTIMING) {
                /* Manual bit timing */
                if (flexcan_validate_bittime_params(&flexcan_bittiming_const, &devinit.bt) != 0) {
                    can_slogf(_SLOG_ERROR, "%s: Invalid manual bitrate settings", __func__);
                    return NULL;
                }
            } else {
                /* Calculate bit timing */
                flexcan_calc_bittiming(&flexcan_bittiming_const, &devinit.bt);
            }

            if (devinit.flags & FLEXCAN_FLAGS_MDTIMING) {
                can_slogf(_SLOG_ERROR, "%s: Warning! manual data bitrate setting is not supported", __func__);
            }
        }

        /* Ensure message ID is valid */
        if (devinit.flags & FLEXCAN_FLAGS_EXTENDED_MID) {
            devinit.midrx &= MID_MASK_EXT;
            devinit.midtx &= MID_MASK_EXT;
        } else {
            devinit.midrx &= MID_MASK_STD;
            devinit.midtx &= MID_MASK_STD;
        }

        /* Process ports and interrupt */
        while ((optind < argc) && (*(argv[optind]) != '-')) {
            optarg = argv[optind];
            if (strncmp(optarg, "can", 3) == 0) {
                flexcan_hwinfo_t canhwinfo = { .regbase = -1,
                                               .numirq = 0,
                                               .irqvector = { [0] = -1, [1] = -1, [2] = -1 } };
                cp = optarg + 3;
                if ((*cp < '0') || (*cp > '9')) {
                    can_slogf(_SLOG_ERROR, "%s: Invalid can options[%s]", __func__, cp);
                    return NULL;
                }
                uid = (int)strtoul(cp, &cp, 0);
                /* Get the CANx interface parameters from HWINFO */
                const int hwi_can = flexcan_get_hwinfo(&canhwinfo, uid);
                if ((hwi_can == -1) || (canhwinfo.regbase == (paddr_t)-1)) {
                    can_slogf(_SLOG_ERROR, "%s: Invalid options[%s]", __func__, optarg);
                    return NULL;
                }

                devinit.port = canhwinfo.regbase;
                /* Only use IRQs from system page if IRQ is not specified from command line */
                if (devinit.numirqs == 0) {
                    devinit.numirqs = canhwinfo.numirq;
                    for (int i = 0; i < canhwinfo.numirq; i++) {
                        devinit.irqsys[i] = canhwinfo.irqvector[i];
                    }
                }
                /* Set default can unit number. Using uid if not set in command option. */
                if (devinit.cinit.can_unit == -1) {
                    devinit.cinit.can_unit = uid;
                }

                /* Increment optarg */
                optarg = cp;
            } else {
                can_slogf(_SLOG_ERROR, "%s: Invalid options[%s]", __func__, optarg);
                return NULL;
            }

            /* Set system interrupt vector
             * This option has highest priority over system page and -r option
             */
            if (*optarg == ',') {
                devinit.numirqs = 0;
                devinit.irqsys[devinit.numirqs++] = (int)strtoul(optarg + 1, &cp, 0);
                while ((cp != NULL) && (*cp == ',')) {
                    if (devinit.numirqs >= FLEXCAN_MAX_IRQ) {
                        can_slogf(_SLOG_ERROR, "%s: Too many IRQs", __func__);
                        return NULL;
                    }
                    devinit.irqsys[devinit.numirqs++] = (int)strtoul(cp + 1, &cp, 0);
                }
            }

            if (devinit.numirqs == 0) {
                can_slogf(_SLOG_ERROR, "%s: IRQ is not assigned!", __func__);
                return NULL;
            }

            found++;

            /* Create the CAN device */
            /* Only handle one CAN interface per driver instance */
            break;
        }

        if (found != 0) break;
    }

    if ((devinit.port == (paddr_t)-1) || (devinit.numirqs == 0)) {
        can_slogf(_SLOG_ERROR, "%s: Port and/or IRQ not assigned!", __func__);
        return NULL;
    }

    if (devinit.cinit.can_unit == -1) {
        devinit.cinit.can_unit = 0;
    }

    if (devinit.cinit.mode != CANDEV_MODE_RAW_FRAME) {
        if (devinit.flags & FLEXCAN_FLAGS_CANFD) {
            devinit.numtx = FLEXCANFD_NUM_MB / 2;
            devinit.numrx = FLEXCANFD_NUM_MB / 2;
        } else {
            devinit.numtx = FLEXCAN_NUM_MB / 2;
            devinit.numrx = FLEXCAN_NUM_MB / 2;
        }
        if (devinit.flags & FLEXCAN_FLAGS_DUALMB) {
            devinit.numtx <<= 1;
            devinit.numrx <<= 1;
        }
    }

    return flexcan_create_device(&devinit);
}

/**
 *  @brief              Create FLEXCAN devices
 *  @param  devinit     Pointer to flexcan_init_t structure
 *
 *  @return             Pointer to flexcan_t  --success
 *                      NULL --failure
 */
static flexcan_info_t* flexcan_create_device(flexcan_init_t* const devinit)
{
    static can_drvr_funcs_t drvr_funcs = { .transmit = flexcan_transmit,
                                           .devctl = flexcan_devctl,
                                           .transmit_raw = NULL,
                                           .receive_raw = NULL,
                                           .event_handler = flexcan_event_handler };
    flexcan_info_t  *devinfo;
    flexcan_t       *devlist;
    int             ret = -1;
    int             rx_dev_unit_num;
    int             tx_dev_unit_num;

    if (devinit->flags & FLEXCAN_FLAGS_IOPRIV) {
        if (ThreadCtl(_NTO_TCTL_IO_PRIV, NULL) == -1) {
            can_slogf(_SLOG_CRITICAL, "%s: Unable to gain IO privity", __func__);
            return NULL;
        }
    }

    if (devinit->verbosity > _SLOG_SEVMAXVAL) {
        devinit->verbosity = _SLOG_SEVMAXVAL;
    }

    if (can_resmgr_init(&drvr_funcs, (int)devinit->verbosity) < 0) {
        can_slogf(_SLOG_CRITICAL, "%s: can_resmgr_init failed", __func__);
        return NULL;
    }

    if (devinit->verbosity >= _SLOG_INFO) {
        can_slogf(_SLOG_INFO, "%s: port = 0x%lX", __func__, devinit->port);
        can_slogf(_SLOG_INFO, "%s: clk = %d", __func__, devinit->bt.freq);
        can_slogf(_SLOG_INFO, "%s: bitrate = %d", __func__, devinit->bt.bitrate);
        can_slogf(_SLOG_INFO, "%s: samplepoint = %d", __func__, devinit->bt.sample_point);
        can_slogf(_SLOG_INFO, "%s: brp = 0x%x", __func__, devinit->bt.brp);
        can_slogf(_SLOG_INFO, "%s: propseg = 0x%x", __func__, devinit->bt.prop_seg);
        can_slogf(_SLOG_INFO, "%s: sjw = 0x%x", __func__, devinit->bt.sjw);
        can_slogf(_SLOG_INFO, "%s: pseg1 = 0x%x", __func__, devinit->bt.phase_seg1);
        can_slogf(_SLOG_INFO, "%s: pseg2 = 0x%x", __func__, devinit->bt.phase_seg2);
        if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
            can_slogf(_SLOG_INFO, "%s: fd_clk = %d", __func__, devinit->dbt.freq);
            can_slogf(_SLOG_INFO, "%s: fd_bitrate = %d", __func__, devinit->dbt.bitrate);
            can_slogf(_SLOG_INFO, "%s: fd_samplepoint = %d", __func__, devinit->dbt.sample_point);
            can_slogf(_SLOG_INFO, "%s: fd_brp = 0x%x", __func__, devinit->dbt.brp);
            can_slogf(_SLOG_INFO, "%s: fd_propseg = 0x%x", __func__, devinit->dbt.prop_seg);
            can_slogf(_SLOG_INFO, "%s: fd_sjw = 0x%x", __func__, devinit->dbt.sjw);
            can_slogf(_SLOG_INFO, "%s: fd_pseg1 = 0x%x", __func__, devinit->dbt.phase_seg1);
            can_slogf(_SLOG_INFO, "%s: fd_pseg2 = 0x%x", __func__, devinit->dbt.phase_seg2);
        }
        for (int i = 0; i < FLEXCAN_MAX_IRQ; i++) {
            can_slogf(_SLOG_INFO, "%s: irqsys[%d] = %d", __func__, i, devinit->irqsys[i]);
        }
        can_slogf(_SLOG_INFO, "%s: unit = %u", __func__, devinit->cinit.can_unit);
        can_slogf(_SLOG_INFO, "%s: flags = %u", __func__, devinit->flags);
        can_slogf(_SLOG_INFO, "%s: numrx = %u", __func__, devinit->numrx);
        can_slogf(_SLOG_INFO, "%s: numtx = %u", __func__, devinit->numtx);
        can_slogf(_SLOG_INFO, "%s: midrx = 0x%X", __func__, devinit->midrx);
        can_slogf(_SLOG_INFO, "%s: midtx = 0x%X", __func__, devinit->midtx);
    }

    /* Allocate device info */
    devinfo = calloc(sizeof(*devinfo), 1);
    if (devinfo == NULL) {
        can_slogf(_SLOG_CRITICAL, "%s: devinfo: calloc failed", __func__);
        flexcan_fini(NULL);
        return NULL;
    }

    /* Set up CAN operation mode - single RX and TX mailboxes using raw frames
     * or multi-mailbox, I/O based communications
     */
    devinfo->mode = devinit->cinit.mode;

    devinfo->base = (uintptr_t)MAP_FAILED;
    devinfo->iidsys[0] = -1;

    /* Setup the RX and TX mailbox sizes */
    devinfo->numrx = devinit->numrx;
    devinfo->numtx = devinit->numtx;

    rx_dev_unit_num = 0;
    tx_dev_unit_num = rx_dev_unit_num + (int)devinit->numrx;

    /* Allocate an array of devices - one for each mailbox */
    devlist = calloc(sizeof(*devlist), devinfo->numrx + devinfo->numtx);
    if (devlist == NULL) {
        can_slogf(_SLOG_CRITICAL, "%s: devlist: calloc failed", __func__);
        flexcan_fini(devinfo);
        return NULL;
    }

    /* Map device registers */
    devinfo->base = (uintptr_t)mmap(NULL, FLEXCAN_SIZE,
                    PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_SHARED | MAP_PHYS, NOFD, (long)devinit->port);
    if (devinfo->base == (uintptr_t)MAP_FAILED) {
        can_slogf(_SLOG_CRITICAL, "%s: CAN REG: Can't map device I/O", __func__);
        flexcan_fini(devinfo);
        return NULL;
    }

    /* Device message memory */
    devinfo->canmsg = (can_msg_obj_t *)(devinfo->base + FLEXCAN_MEM_OFFSET);
    memset(devinfo->canmsg, 0, FLEXCAN_MEM_SIZE);

    /* CANLAM memory */
    devinfo->canlam = devinfo->base + FLEXCAN_RXIMR0;

    /* FD registers */
    if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
        devinfo->base_fd = devinfo->base + FLEXCAN_FDCTRL;
    }

    /* Setup device info */
    devinfo->devlist = devlist;
    strcpy(devinfo->initinfo.description,"FlexCAN");
    devinfo->initinfo.msgq_size = devinit->cinit.msgq_size;
    devinfo->initinfo.waitq_size = devinit->cinit.waitq_size;
    devinfo->initinfo.bt.bitrate = devinit->bt.bitrate;
    devinfo->initinfo.bt.brp = devinit->bt.brp;
    devinfo->initinfo.bt.sjw = devinit->bt.sjw;
    devinfo->initinfo.bt.phase_seg1 = devinit->bt.phase_seg1;
    devinfo->initinfo.bt.phase_seg2 = devinit->bt.phase_seg2;
    devinfo->initinfo.bt.prop_seg = devinit->bt.prop_seg;
    if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
        devinfo->initinfo.dbt.bitrate = devinit->bt.bitrate;
        devinfo->initinfo.dbt.brp = devinit->dbt.brp;
        devinfo->initinfo.dbt.sjw = devinit->dbt.sjw;
        devinfo->initinfo.dbt.phase_seg1 = devinit->dbt.phase_seg1;
        devinfo->initinfo.dbt.phase_seg2 = devinit->dbt.phase_seg2;
        devinfo->initinfo.dbt.prop_seg = devinit->dbt.prop_seg;
    }
    /* Initialize flags */
    devinfo->flags = devinit->flags;

    devinfo->num_mailboxes = devinit->numrx + devinit->numtx;

    /* Initialize all device mailboxes */
    uint32_t mbxid;
    for (mbxid = 0; mbxid < devinfo->num_mailboxes; mbxid++) {
        /* Set index into device mailbox memory */
        devlist[mbxid].mbxid = mbxid;
        /* Store a pointer to the device info */
        devlist[mbxid].devinfo = devinfo;
        /* Default the resmgr device unit number. It will get overwritten later
         * for those mailboxes which actually get exposed through the resmgr.
         */
        devlist[mbxid].cdev.dev_unit = -1;
        devinfo->devlist[mbxid].cdev.devtype = -1;

        if (devinit->flags & FLEXCAN_FLAGS_CANFD) {
            devlist[mbxid].cdev.cflags |= CANDEV_CFLAG_CANFD;
        }

        /* Only expose the 'USER' mailboxes to the resource manager. */
        if (mbxid < (devinfo->numrx + devinfo->numtx)) {
            /* Set device mailbox as transmit or receive and set resmgr unit number */
            if (mbxid < devinfo->numrx) {
                devinit->cinit.devtype = CANDEV_TYPE_RX;
                devinit->cinit.dev_unit = rx_dev_unit_num++;
            } else {
                devinit->cinit.devtype = CANDEV_TYPE_TX;
                devinit->cinit.dev_unit = tx_dev_unit_num++;
            }

            /* Initialize the CAN device */
            ret = can_resmgr_init_device(&devlist[mbxid].cdev, (CANDEV_INIT *)devinit);
            if (ret == -1) break;

            /* Create the resmgr device */
            ret = can_resmgr_create_device(&devlist[mbxid].cdev);
            if (ret == -1) {
                can_resmgr_destroy_device(&devlist[mbxid].cdev);
                break;
            }
        }
    }

    if (ret == -1) {
        devinfo->num_mailboxes = mbxid;     /* To make cleanup routine happy */
    } else {
        if (flexcan_init_hw(devinfo, devinit) == 0) {
            if (flexcan_init_intr(devinfo, devinit) == 0) return devinfo;
        }
    }

    /* cleanup */
    flexcan_fini(devinfo);

    return NULL;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/can/flexcan/driver.c $ $Rev: 985670 $")
#endif
