/*
 * Copyright (c) 2013, 2023, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, 2020, NXP.
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
 *
 */

#include <sim_sdmmc.h>

#include "sim_bs.h"
#include "imx8_hc.h"
#include "hw/dcmd_nxpmx8x_sdmmc.h"

/**
 * Board specific interface
 *
 * @file       sdmmc/aarch64/mx8x.le/sim_bs.c
 * @addtogroup sdmmc_bs
 * @{
 */

/**
 * Strobe DLL slave update interval devctl command.
 *
 * @param hba HBA pointer.
 * @param ccb CCB pointer.
 *
 * @return CAM_REQ_CMP always.
 */
static int sdmmc_host_sdll_devctl( SIM_HBA *const hba, CCB_DEVCTL *ccb )
{
    SIM_SDMMC_EXT       *const ext = (SIM_SDMMC_EXT *)hba->ext;
    struct sdio_device  *const device = ext->device;
    sdio_hc_t           *const hc = device->hc;
    int                 status;
    unsigned            val = 0;
    const uint32_t            dtype = ext->dev_inf.dtype; /* Device Type */

    if (dtype != DEV_TYPE_MMC) {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "%s:  Unsupported device type: %d", __FUNCTION__, dtype);
        ccb->cam_devctl_status = ENOTSUP;
        return (CAM_REQ_CMP);
    }
    if (sdmmc_unit_ready( hba, (CCB_SCSIIO *)ccb ) != CAM_REQ_CMP) {
        ccb->cam_devctl_status = EIO;
        return (CAM_REQ_CMP);
    }

    val = *(unsigned *)ccb->cam_devctl_data;
    status = imx_sdhcx_host_sdll(hc, val);
    slogf(_SLOGC_SIM_MMC, _SLOG_INFO, "%s:  Runtime sdll value : 0x%X, status %d", __FUNCTION__, val, status);
    ccb->cam_devctl_status = status;

    return (CAM_REQ_CMP);
}

/**
 * Board specific arguments.
 *
 * @param hba       HBA pointer.
 * @param options   Board specific options.
 *
 * @return EOK always.
 */
int sim_bs_args(SIM_HBA *const hba, char *options)
{
    SIM_SDMMC_EXT   *ext;
    char            *value;
    int              opt;
    static char     *opts[] = {
                        "dname",
                        NULL
                    };

    ext = (SIM_SDMMC_EXT *)hba->ext;

    while (options && (*options != '\0')) {
        opt = getsubopt( &options, opts, &value);
        if (opt == -1) {
            /* Nothing to do */
        }
        switch (opt) {
            case 0:
                ext->eflags |= SDMMC_EFLAG_DEVNAME;
                if (value && !strncmp(value, "compat", strlen("compat"))) {
                    ext->eflags |= SDMMC_EFLAG_BS;
                }
                break;
            default:
                break;
        }
    }

    return (EOK);
}

/**
 * Board specific devctl commands.
 *
 * @param hba HBA pointer.
 * @param ccb CCB pointer.
 *
 * @return CAM_REQ_CMP always.
 */
int sim_bs_devctl(SIM_HBA *const hba, CCB_DEVCTL *ccb)
{
    int status;

    status = CAM_REQ_CMP;

    switch (ccb->cam_devctl_dcmd) {

    case DCMD_NXP_SDMMC_HOST_SDLL:
        if( !( ccb->cam_devctl_ioflag & ( _IO_FLAG_RD | _IO_FLAG_WR ) ) ) {
            cam_slogf( _SLOGC_SIM_MMC, _SLOG_ERROR, 1, 1, "%s:  dcmd 0x%x, EACCES", __FUNCTION__, ccb->cam_devctl_dcmd );
            ccb->cam_devctl_status = EACCES;
            break;
        }
        status = sdmmc_host_sdll_devctl(hba, ccb);
        break;
    default:
        break;
    }

    return (status);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devb/sdmmc/aarch64/mx8x.le/sim_bs.c $ $Rev: 976550 $")
#endif
