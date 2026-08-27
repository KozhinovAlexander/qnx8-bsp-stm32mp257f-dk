/*
 * $QNXLicenseC:
 * Copyright 2022 BlackBerry Limited.
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

#include "ipl.h"
#include "board.h"
#include "imx_ipl.h"
#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_smc_call.h>
#include <soc/nxp/imx8/mp/imx_ocotp.h>

#define IMX_HAB_CID_ROM                 0     /* ROM Caller ID */
#define IMX_HAB_CID_UBOOT               1     /* UBOOT Caller ID*/

#define IMX_HAB_TAG_RVT                 0xDD  /* ROM Vector Table */
#define IMX_HAB_CMD_HDR                 0xD4  /* CSF Header */
#define IMX_HAB_CMD_WRT_DAT             0xCC  /* Write Data command tag */
#define IMX_HAB_CMD_CHK_DAT             0xCF  /* Check Data command tag */
#define IMX_HAB_CMD_SET                 0xB1  /* Set command tag */
#define IMX_HAB_PAR_MID                 0x01  /* MID parameter value */

/* HAB functions */
#define IMX_HAB_RVT_ENTRY               (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_ENTRY_OFFSET))
#define IMX_HAB_RVT_EXIT                (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_EXIT_OFFSET))
#define IMX_HAB_RVT_CHECK_TARGET        (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_CHECK_TARGET_OFFSET))
#define IMX_HAB_RVT_AUTHENTICATE_IMAGE  (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_AUTHENTICATE_IMAGE_OFFSET))
#define IMX_HAB_RVT_REPORT_EVENT        (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_REPORT_EVENT_OFFSET))
#define IMX_HAB_RVT_REPORT_STATUS       (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_REPORT_STATUS_OFFSET))
#define IMX_HAB_RVT_FAILSAFE            (*(uint64_t *)(IMX_HAB_RVT_BASE + IMX_HAB_RVT_FAILSAFE_OFFSET))

/* Status definitions */
enum hab_status {
    HAB_STS_ANY         = 0x00,
    HAB_FAILURE         = 0x33,
    HAB_WARNING         = 0x69,
    HAB_SUCCESS         = 0xF0
};

/* Security Configuration definitions */
enum hab_config {
    HAB_CFG_RETURN      = 0x33,  /* Field Return IC */
    HAB_CFG_OPEN        = 0xF0,  /* Non-secure IC */
    HAB_CFG_CLOSED      = 0xCC   /* Secure IC */
};

/* State definitions */
enum hab_state {
    HAB_STATE_INITIAL       = 0x33, /* Init. state */
    HAB_STATE_CHECK         = 0x55, /* Check state */
    HAB_STATE_NONSECURE     = 0x66, /* Non-secure state */
    HAB_STATE_TRUSTED       = 0x99, /* Trusted state */
    HAB_STATE_SECURE        = 0xAA, /* Secure state */
    HAB_STATE_FAIL_SOFT     = 0xCC, /* Soft fail state */
    HAB_STATE_FAIL_HARD     = 0xFF, /* Hard fail state */
    HAB_STATE_NONE          = 0xF0, /* No security state machine */
    HAB_STATE_MAX
};

enum hab_target {
    HAB_TGT_MEMORY          = 0x0F,
    HAB_TGT_PERIPHERAL      = 0xF0,
    HAB_TGT_ANY             = 0x55
};

typedef ulong ptrdiff_t;

typedef enum hab_status hab_rvt_entry_t(void);
typedef enum hab_status hab_rvt_exit_t(void);
typedef enum hab_status hab_rvt_check_target_t(enum hab_target, const void *, size_t);
typedef enum hab_status hab_loader_callback_f_t(void**, size_t*, const void*);
typedef void *hab_rvt_authenticate_image_t(uint8_t, ptrdiff_t, void **, size_t *, hab_loader_callback_f_t);

typedef enum hab_status hab_rvt_report_status_t(enum hab_config *, enum hab_state *);
typedef enum hab_status hab_rvt_report_event_t(enum hab_status, uint32_t, uint8_t* , size_t*);

struct ivt_header {
    uint8_t     magic;
    uint16_t    length;
    uint8_t     version;
} __attribute__((packed));

struct ivt {
    struct ivt_header hdr;  /* IVT header */
    uint32_t entry;         /* Absolute address of first instruction */
    uint32_t reserved1;     /* Reserved should be zero */
    uint32_t dcd;           /* Absolute address of the image DCD */
    uint32_t boot;          /* Absolute address of the boot data */
    uint32_t self;          /* Absolute address of the IVT */
    uint32_t csf;           /* Absolute address of the CSF */
    uint32_t reserved2;     /* Reserved should be zero */
};

struct hab_hdr {
    uint8_t tag;             /* Tag field */
    uint8_t len[2];          /* Length field in bytes (big-endian) */
    uint8_t par;             /* Parameters field */
} __attribute__((packed));

#if (IMX_HAB_AUTHENTICATE_CONFIG == 1)
/**
 * HAB entry function.
 *
 * @return HAB entry function status.
 */
static enum hab_status imx_hab_rvt_entry(void)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_entry_t *hab_rvt_entry_func;
    hab_rvt_entry_func = (hab_rvt_entry_t *)IMX_HAB_RVT_ENTRY;

    return hab_rvt_entry_func();
#else
    return (enum hab_status)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_ENTRY, 0x00, 0x00, 0x00);
#endif
}

/**
 * Check target for HAB using.
 *
 * @param type  HAB type.
 * @param start Pointer to image address.
 * @param bytes Size of image.
 *
 * @return HAB check target status.
 */
static enum hab_status imx_hab_rvt_check_target(enum hab_target type, const void *start, size_t bytes)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_check_target_t *hab_rvt_check_target_func;
    hab_rvt_check_target_func =  (hab_rvt_check_target_t *)IMX_HAB_RVT_CHECK_TARGET;

    return hab_rvt_check_target_func(type, start, bytes);
#else
    return (enum hab_status)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_CHECK_TARGET, (uint64_t)type, (uint64_t)start, (uint64_t)bytes);
#endif
}

/**
 * Authenticate image by HAB algorithm.
 *
 * @param cid        CID type.
 * @param ivt_offset IVT offset.
 * @param start      Image address start.
 * @param bytes      Image size.
 * @param loader     Address to HAB callback.
 *
 * @return HAB authenticate image status.
 */
static void *imx_hab_rvt_authenticate_image(uint8_t cid, ptrdiff_t ivt_offset,
        void **start, size_t *bytes, hab_loader_callback_f_t loader)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_authenticate_image_t *hab_rvt_authenticate_image_func;
    hab_rvt_authenticate_image_func = (hab_rvt_authenticate_image_t *)IMX_HAB_RVT_AUTHENTICATE_IMAGE;

    return hab_rvt_authenticate_image_func(cid, ivt_offset, start, bytes, loader);
#else
    return (void *)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_AUTHENTICATE, (uint64_t)ivt_offset, (uint64_t)start, (uint64_t)bytes);
#endif
}

/**
 * HAB exit function.
 *
 * @return HAB exit function status.
 */
static enum hab_status imx_hab_rvt_exit(void)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_exit_t *hab_rvt_exit_func;
    hab_rvt_exit_func =  (hab_rvt_exit_t *)IMX_HAB_RVT_EXIT;

    return hab_rvt_exit_func();
#else
    return (enum hab_status)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_EXIT, 0x00, 0x00, 0x00);
#endif
}

/**
 * Get HAB report status.
 *
 * @param config Pointer to security configuration definitions variable.
 * @param state  Pointer to state definitions variable.
 *
 * @return      HAB report status.
 */
static enum hab_status imx_hab_rvt_report_status(enum hab_config *config, enum hab_state *state)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_report_status_t *hab_rvt_report_status_func;
    hab_rvt_report_status_func = (hab_rvt_report_status_t *)IMX_HAB_RVT_REPORT_STATUS;

    return hab_rvt_report_status_func(config, state);
#else
    return (enum hab_status)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_REPORT_STATUS, (uint64_t)config, (uint64_t)state, 0x00);
#endif
}

/**
 * Get HAB status event and event data.
 *
 * @param status Status event type.
 * @param index  Status event index.
 * @param event  Pointer to event buffer.
 * @param bytes  Pointer to event buffer buffer size.
 *
 * @return       Status of getting event HAB operation.
 */
static enum hab_status imx_hab_rvt_report_event(enum hab_status status, uint32_t index, uint8_t *event, size_t *bytes)
{
#if defined(IMX_SPL_BOOT)
    hab_rvt_report_event_t *hab_rvt_report_event_func;
    hab_rvt_report_event_func =  (hab_rvt_report_event_t *)IMX_HAB_RVT_REPORT_EVENT;

    return hab_rvt_report_event_func(status, index, event, bytes);
#else
    return (enum hab_status)imx_sec_firmware_psci(IMX_FSL_SIP_HAB, IMX_FSL_SIP_HAB_REPORT_EVENT, (uint64_t)index, (uint64_t)event, (uint64_t)bytes);
#endif
}

static int imx_is_hab_enabled(void)
{
    uint32_t fuse = in32(IMX_OCOTP_CTRL_BASE + IMX_HAB_SEC_CONFIG);

    if (!((fuse & IMX_HAB_SEC_CONFIG_ENABLE_BIT) == IMX_HAB_SEC_CONFIG_ENABLE_BIT)) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Print event data in hex code.
 *
 * @param event_data Pointer to event data buffer.
 * @param bytes      Event data count.
 */
static void display_event(uint8_t *event_data, size_t bytes)
{
    uint32_t i;

    if (!(event_data && bytes > 0)) {
        return;
    }

    for (i = 0; i < bytes; i++) {
        if (i == 0) {
            kprintf("\t0x%x", event_data[i]);
        } else if ((i % 8) == 0) {
            kprintf("\n\t0x%x", event_data[i]);
        } else {
            kprintf(" 0x%x", event_data[i]);
        }
    }
}

/**
 * Get HAB status, print HAB events.
 */
static void imx_get_hab_status(void)
{
    uint32_t index = 0;         /* Loop index */
    uint8_t event_data[128];    /* Event data buffer */
    size_t bytes = sizeof(event_data); /* Event size in bytes */
    enum hab_config config = 0;
    enum hab_state state = 0;

    if (imx_is_hab_enabled()) {
        kprintf("\nSecure boot is enabled\n");
    } else {
        kprintf("\nSecure boot is disabled\n");
    }

    /* Check HAB status */
    if (imx_hab_rvt_report_status(&config, &state) != HAB_SUCCESS) {
        kprintf("\nHAB Configuration: 0x%x", config);
        kprintf(" , HAB State: 0x%x", state);

        /* Display HAB events */
        while (imx_hab_rvt_report_event(HAB_STS_ANY, index, event_data, &bytes) == HAB_SUCCESS) {
            kprintf("\n--------- HAB Event %d-----------------\n", (index + 1);
            kprintf("event data:\n");
            display_event(event_data, bytes);
            kprintf("\n");
            bytes = sizeof(event_data);
            index++;
        }
    } else {
        kprintf("HAB Configuration: 0x%x, HAB State: 0x%x\n", config, state);
        kprintf("No HAB Events Found!\n\n");
    }

}

/**
 * Check validity of IVT structure.
 *
 * @param ivt Pointer to IVT structure.
 *
 * @return IVT validity status.
 */
static int imx_is_ivt_valid(struct ivt *ivt)
{
    struct ivt_header *ivt_hdr = &ivt->hdr;
    int result = 0;

    if (ivt_hdr->magic != IMX_IVT_HEADER_MAGIC) {
        kprintf("bad IVT magic\n");
    } else if ((ivt_hdr->version & IMX_HAB_MAJ_MASK) != IMX_HAB_MAJ_VER) {
        kprintf("bad IVT version\n");
    } else {
        result = 1;
    }

    return result;
}

/**
 * Check validity of CSF structure.
 *
 * @param ivt_addr   Pointer IVT structure.
 * @param start_addr Image start address.
 * @param bytes      Image size.
 *
 * @return CSF validity status.
 */
static int imx_is_csf_valid(struct ivt *ivt, ulong start_addr, size_t bytes)
{

    uint8_t *csf_hdr, *end, *start = (uint8_t *)start_addr;
    size_t csf_hdr_len, cmd_hdr_len, offset = 0;
    struct hab_hdr *cmd;

    end = (bytes != 0) ? (start + bytes - 1) : start;

    /* Verify if CSF pointer content is zero */
    if (ivt->csf == 0U) {
        kprintf("Error: CSF pointer is NULL\n");
        return 0;
    }
    csf_hdr = (uint8_t *)(ulong)ivt->csf;
    /* Verify if CSF Header exist */
    if (*csf_hdr != IMX_HAB_CMD_HDR) {
        kprintf("Error: CSF header command not found\n");
        return 0;
    }
    csf_hdr_len = (size_t)((((struct hab_hdr *)csf_hdr)->len[0] << 8) +
                  (((struct hab_hdr *)csf_hdr)->len[1]));
    /* Check if the CSF lies within the image bounds */
    if ((csf_hdr == 0) || (csf_hdr < start) ||
            (csf_hdr > end) || ((size_t)((end + 1) - csf_hdr) < csf_hdr_len)) {
        kprintf("Error: CSF lies outside the image bounds\n");
        return 0;
    }
    do {
        cmd = (struct hab_hdr *)&csf_hdr[offset];
        switch (cmd->tag) {
        case (IMX_HAB_CMD_WRT_DAT):
            kprintf("Error: Deprecated write command found\n");
            return 0;
        case (IMX_HAB_CMD_CHK_DAT):
            kprintf("Error: Deprecated check command found\n");
            return 0;
        case (IMX_HAB_CMD_SET):
            if (cmd->par == IMX_HAB_PAR_MID) {
                kprintf("Error: Deprecated Set MID command found\n");
                return 0;
            }
            break;
        default:
            break;
        }

        cmd_hdr_len = (csf_hdr[offset] == IMX_HAB_CMD_HDR)? sizeof(struct hab_hdr) :
                     (size_t)((((struct hab_hdr *) &csf_hdr[offset])->len[0] << 8) +
                     (((struct hab_hdr *) &csf_hdr[offset])->len[1]));
        if (cmd_hdr_len == 0U) {
            kprintf("Error: Invalid command length\n");
            return 0;
        }
        offset += cmd_hdr_len;
    } while (offset < csf_hdr_len);

    return 1;
}

/**
 * Authenticate image stored in DDR memory.
 *
 * @param img_addr   Image DDR memory address.
 * @param img_size   Image size in bytes.
 * @param ivt_offset IVT structure offset from img_addr address.
 *
 * @return         Authentication of image status.
 * @retval          0   Image was correctly authenticated.
 * @retval         -1   Image was not correctly authenticated.
 */
static int imx_hab_authenticate_image(paddr_t img_addr, uint32_t img_size, uint32_t ivt_offset)
{
    enum hab_status status;
    uint64_t load_addr = 0, start, bytes;
    int ret = -1;

    do {
        /* Verify IVT header bugging out on error */
        if (imx_is_ivt_valid((struct ivt *)(img_addr + ivt_offset)) == 0U) {
            kprintf("IVT validation failed!\n");
            break;
        }
        start = img_addr;
        bytes = img_size;

        /* Verify CSF */
        if (imx_is_csf_valid((struct ivt *)(img_addr + ivt_offset), start, bytes) == 0U) {
            kprintf("CST validation failed!\n");
            break;
        }
        if (imx_hab_rvt_entry() != HAB_SUCCESS) {
            kprintf("HAB entry function fail\n");
            break;
        }
        status = imx_hab_rvt_check_target(HAB_TGT_MEMORY, (void *)(ulong)img_addr, img_size);
        if (status != HAB_SUCCESS) {
            kprintf("HAB check target 0x%x-0x%x\n", img_addr, (img_addr + img_size));
            break;
        }
        load_addr = (uint64_t)imx_hab_rvt_authenticate_image(
                IMX_HAB_CID_UBOOT,
                ivt_offset, (void **)&start,
                (size_t *)&bytes, 0UL);

        if (imx_hab_rvt_exit() != HAB_SUCCESS) {
            kprintf("HAB exit function fail\n");
            load_addr = 0;
            break;
        }
    } while (0);

#if !defined(IMX_SPL_BOOT)
    imx_get_hab_status();
#endif

    if ((load_addr != 0) || !(imx_is_hab_enabled())) {
        ret = 0;
    }

    return ret;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_hab.c $ $Rev: 980064 $")
#endif
