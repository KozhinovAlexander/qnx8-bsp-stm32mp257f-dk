/*
 * Copyright (c) 2022, BlackBerry Limited.
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


/*
** File: f3s_socket_option.c
**
** Description:
**
** This file contains the socket command line option parser
*/

/*
** Includes
*/

#include "f3s_snor.h"

static inline int snor_socket_parse_range(const void* const cs, const char* const fn, const char* const os,
                char *value, const int ws, const uint32_t lo, const uint32_t hi, const uint8_t nc)
{
    uint32_t    val;
    char        *ltok;
    const char* const delims = { ":" };

    value = strtok_r(value, delims, &ltok);
    uint8_t *tc = (uint8_t *)cs;
    uint8_t cnt = 0;
    while (value != NULL) {
        val = (uint32_t)strtoul(value, NULL, 0);
        if ((val > hi) || (val < lo)) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: invalid %s[%x]", fn, os, val);
            return (EINVAL);
        }

        if (++cnt > nc) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: Too many entries %s", fn, os);
            return (EINVAL);
        }
        if (ws == 32) {
            *(uint32_t *)tc = val;
        } else if (ws == 8) {
            *tc = (uint8_t)val;
        } else {
            return (EINVAL);
        }
        tc += sizeof(snor_chip_t);  /* move to next chip */
        value = strtok_r(NULL, delims, &ltok);
    }

    return (EOK);
}

struct _snor_hcaps_opt {
    char        *opts;
    uint64_t    cap;
};

/**
 *  @brief             Handle host controller capability mask.
 *  @param caps        Capability string.
 *  @param hcmask      Host capability mask.
 *
 *  @return            EOK --success otherwise fail.
 */
static inline uint32_t snor_hcaps(const char* const caps, uint64_t* const hcmask)
{
    int     idx, force;

    force = (caps[0] == '~') ? 0 : 1;

    static const struct _snor_hcaps_opt captbl[] = {
        { .opts = "111FAST", .cap = SNOR_HCAPS_RD_1_1_1_FAST | SNOR_HCAPS_PP_1_1_1 },
        { .opts = "111DTR",  .cap = SNOR_HCAPS_RD_1_1_1_DTR | SNOR_HCAPS_PP_1_1_1 },
        { .opts = "111",     .cap = SNOR_HCAPS_RD_1_1_1 |SNOR_HCAPS_PP_1_1_1 },
        { .opts = "112DTR",  .cap = SNOR_HCAPS_RD_1_1_2_DTR | SNOR_HCAPS_PP_1_1_2 },
        { .opts = "122DTR",  .cap = SNOR_HCAPS_RD_1_2_2_DTR | SNOR_HCAPS_PP_1_2_2 },
        { .opts = "222DTR",  .cap = SNOR_HCAPS_RD_2_2_2 | SNOR_HCAPS_DTR_CMD | SNOR_HCAPS_PP_2_2_2},
        { .opts = "112",     .cap = SNOR_HCAPS_RD_1_1_2 | SNOR_HCAPS_PP_1_1_2 },
        { .opts = "122",     .cap = SNOR_HCAPS_RD_1_2_2 | SNOR_HCAPS_PP_1_2_2 },
        { .opts = "222",     .cap = SNOR_HCAPS_RD_2_2_2 | SNOR_HCAPS_PP_2_2_2 },
        { .opts = "114DTR",  .cap = SNOR_HCAPS_RD_1_1_4_DTR | SNOR_HCAPS_PP_1_1_4 },
        { .opts = "144DTR",  .cap = SNOR_HCAPS_RD_1_4_4_DTR | SNOR_HCAPS_PP_1_4_4 },
        { .opts = "444DTR",  .cap = SNOR_HCAPS_RD_4_4_4 | SNOR_HCAPS_DTR_CMD | SNOR_HCAPS_PP_4_4_4 },
        { .opts = "114",     .cap = SNOR_HCAPS_RD_1_1_4 | SNOR_HCAPS_PP_1_1_4 },
        { .opts = "144",     .cap = SNOR_HCAPS_RD_1_4_4 | SNOR_HCAPS_PP_1_4_4 },
        { .opts = "444",     .cap = SNOR_HCAPS_RD_4_4_4 | SNOR_HCAPS_PP_4_4_4 },
        { .opts = "118DTR",  .cap = SNOR_HCAPS_RD_1_1_8_DTR | SNOR_HCAPS_PP_1_1_8 },
        { .opts = "188DTR",  .cap = SNOR_HCAPS_RD_1_8_8_DTR | SNOR_HCAPS_PP_1_8_8 },
        { .opts = "888DTR",  .cap = SNOR_HCAPS_RD_8_8_8 | SNOR_HCAPS_DTR | SNOR_HCAPS_PP_8_8_8 },
        { .opts = "118",     .cap = SNOR_HCAPS_RD_1_1_8 | SNOR_HCAPS_PP_1_1_8 },
        { .opts = "188",     .cap = SNOR_HCAPS_RD_1_8_8 | SNOR_HCAPS_PP_1_8_8 },
        { .opts = "888",     .cap = SNOR_HCAPS_RD_8_8_8 | SNOR_HCAPS_PP_8_8_8 },
    };

#define NELEMENTS(x)        (sizeof((x)) / sizeof((x)[0]))
    for (idx = 0; idx < NELEMENTS(captbl); idx++) {
        if (strcasecmp(captbl[idx].opts, caps) == 0) {
            if (force) {
                *hcmask = captbl[idx].cap;
            } else {
                *hcmask &= ~captbl[idx].cap;
            }
            return (EOK);
        }
    }

    return (EINVAL);
}

/**
 *  @brief             Report invalid suboption.
 *  @param function    Function name.
 *  @param suboption   suboptions.
 *
 *  @return            None.
 */
static void report_invalid_suboption(const char* const function, const char* const suboption)
{
    char text[50];

    int i;

    for (i = 0; i < (sizeof text) - 1; i++) {
        const char ch = suboption[i];
        if (!ch || (ch == ',')) break;

        text[i] = ch;
    }
    text[i] = '\0';

    snor_slogf(_SLOG_ERROR, 0, 0, "%s: Invalid socket option: %s", function, text);
}

/**
 *  @brief             Handle socket option.
 *  @param socket      Socket handle
 *
 *  @return            EOK --success otherwise fail.
 */
int f3s_socket_option(f3s_socket_t *socket)
{
    snor_ctrl_t *ctrl = (snor_ctrl_t *)socket->memory;
    char        *options, *value, *freeptr;
//    int         opt, val;
    int         opt;
    int         status = EOK;
    char        *ltok;
    const char* const delims = { ":" };
    int         check_pow2;
    enum
    {
        OPTION_VERBOSITY = 0,
        OPTION_UNIT_SIZE,
        OPTION_SOC_SPECIFIC,
        OPTION_VID,         /* Per flash device */
        OPTION_DID,         /* per flash device */
        OPTION_DATA_RATE,   /* Per flash device */
        OPTION_PROTOCOL,    /* Per flash device */
        OPTION_DRV_STRENGTH,/* Per flash device */
        OPTION_NUM_CS,
        OPTION_STRIPE,
        OPTION_HYPER,
    };
    static char *opts[] =
    {
        [OPTION_VERBOSITY] = "verbose",
        [OPTION_UNIT_SIZE] = "unit_size",
        [OPTION_SOC_SPECIFIC] = "soc",
        [OPTION_VID] = "vid",
        [OPTION_DID] = "did",
        [OPTION_DATA_RATE] = "drate",
        [OPTION_PROTOCOL] = "protocol",
        [OPTION_DRV_STRENGTH] = "drv_strength",
        [OPTION_NUM_CS] = "numcs",
        [OPTION_STRIPE] = "stripe",
        [OPTION_HYPER] = "hyper",
        NULL
    };

    /* check if there is no socket option */
    if (socket->option == NULL) return ENOENT;

    freeptr = strdup((char *)socket->option);
    if (freeptr == NULL) return (ENOMEM);
    options = freeptr;

    while (options && (*options != '\0') && (status == EOK)) {
        opt = getsubopt(&options, opts, &value);
        if (opt == -1) {
            /* Encountered an invalid option. */
            report_invalid_suboption(__func__, value);
            status = EINVAL;
            break;
        }

        switch (opt) {
            case OPTION_VERBOSITY:          /* verbosity */
                if ((value != NULL) && (*value != '\0')) {
                    ctrl->verbosity = (int)strtol(value, NULL, 0);
                } else {
                    ctrl->verbosity++;
                }
                break;

            case OPTION_UNIT_SIZE:          /* unit size */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status != EOK) break;
                socket->unit_size = (uint32_t)strtoul(value, &value, 0);
                if ((*value == 'K') || (*value == 'k')) {
                    socket->unit_size *= 1024;
                }
                break;

            case OPTION_SOC_SPECIFIC:       /* SoC options */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status == EOK) {
                    char* const soc_opts = strdup(value);
                    if (soc_opts == NULL) {
                        status = errno;
                    } else {
                        ctrl->soc_opts = soc_opts;  /* will be freed in SoC module */
                    }
                }
                break;

            case OPTION_VID:                /* vendor ID */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status == EOK) {
                    status = snor_socket_parse_range(&ctrl->chip[0].vid, __func__, "Vendor ID", value, 8, 1, 254, SNOR_MAX_CS);
                }
                break;

            case OPTION_DID:                /* device ID */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status == EOK) {
                    status = snor_socket_parse_range(&ctrl->chip[0].did, __func__, "Device ID", value, 8, 1, 254, SNOR_MAX_CS);
                }
                break;

            case OPTION_DATA_RATE:           /* bus clock */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status == EOK) {
                    status = snor_socket_parse_range(&ctrl->chip[0].cfg.clk, __func__, "Data rate", value, 32, 1000, 2000000000, SNOR_MAX_CS);
                }
                break;

            case OPTION_PROTOCOL:            /* bus protocol */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status != EOK) break;
                value = strtok_r(value, delims, &ltok);
                snor_chip_t *chip = &ctrl->chip[0];
                while (value != NULL) {
                    status = (int)snor_hcaps(value, &chip->hcmask);
                    if (status != EOK) {
                        snor_slogf(_SLOG_ERROR, 0, 0, "%s: Unknown bus protocol[%s]", __func__, value);
                        break;
                    }
                    chip++;

                    value = strtok_r(NULL, delims, &ltok);
                }
                break;

            case OPTION_DRV_STRENGTH:       /* Driver strength */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status != EOK) break;
                status = snor_socket_parse_range(&ctrl->chip[0].drv_type, __func__, "Driver strength", value, 8, 0, 255, SNOR_MAX_CS);
                break;

            case OPTION_NUM_CS:             /* number of chip select */
                status = snor_options_arg_value(__func__, opts[opt], value);
                if (status != EOK) break;
                status = snor_socket_parse_range(&ctrl->ncs, __func__, "Number of chip select", value, 8, 1, SNOR_MAX_CS, 1);
                break;

            case OPTION_STRIPE:             /* Stripe mode */
                status = snor_options_arg_novalue(__func__, opts[opt], value);
                if (status == EOK) {
                    ctrl->flags |= SNOR_FLG_STRIPE;
                }
                break;

            case OPTION_HYPER:              /* Hyperflash */
                status = snor_options_arg_novalue(__func__, opts[opt], value);
                if (status == EOK) {
                    ctrl->flags |= SNOR_FLG_HYPER;
                }
                break;

            default:
                break;
        }

        if (status != EOK) break;
    }

    if (freeptr != NULL) {
        free(freeptr);
    }

    if (status != EOK) {
        if (ctrl->soc_opts != NULL) {
            free(ctrl->soc_opts);
            ctrl->soc_opts = NULL;
        }
        return (status);
    }

    /* find power of two of unit size */
    if (socket->unit_size) {
        check_pow2 = 1;
        while (((1u << check_pow2) != socket->unit_size) && (check_pow2 < 32)) {
            check_pow2++;
        }

        /* check if unit size is not a power of two */
        if (check_pow2 >= 32) {
            errno = EINVAL;
            perror("mtd-snor: socket unit size must be a power of two");
            exit(errno);
        }
    }

    /* everything is fine */
    return (EOK);
}

/*
** End
*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
