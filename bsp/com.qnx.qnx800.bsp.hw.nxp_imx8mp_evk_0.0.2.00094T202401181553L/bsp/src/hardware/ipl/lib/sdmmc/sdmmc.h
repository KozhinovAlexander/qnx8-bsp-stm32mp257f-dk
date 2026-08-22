/*
 * Copyright (c) 2022-2023, BlackBerry Limited.
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

#ifndef _SDMMC_H_INCLUDE
#define _SDMMC_H_INCLUDE

/*
 * SDMMC Block Size
 */
#define SDMMC_BLOCKSIZE             512

/*
 * SDMMC error codes
 */
#define SDMMC_OK                    0       // no error
#define SDMMC_ERROR                 (-1)      // SDMMC error

/* Max clock frequencies (Hz) */
#define DTR_MAX_SDR104              208000000
#define DTR_MAX_SDR50               100000000
#define DTR_MAX_DDR50               50000000
#define DTR_MAX_SDR25               50000000
#define DTR_MAX_SDR12               25000000
#define DTR_MAX_HS200               200000000
#define DTR_MAX_HS400               200000000
#define DTR_MAX_HS52                52000000
#define DTR_MAX_HS26                26000000

/*
 * SDMMC card type
 */
typedef enum
{
    NONE = 0, eMMC, eSDC, eSDC_V200, eSDC_HC, eMMC_HC
} card_type_t;

/*
 * SDMMC command structure
 */
typedef struct
{
    int         cmd;        // command to be issued
    uint32_t    arg;        // command argument
    uint32_t    *rsp;       // pointer to response buffer
    uint32_t    bsize;      // data block size
    uint32_t    bcnt;       // data block count
    void        *dbuf;      // pointer to data buffer
    uint32_t    flgsts;     // flag to HC, error status from HC
} sdmmc_cmd_t;

#define IS_EMMC_CARD(_card)         (((_card)->type == eMMC) || ((_card)->type == eMMC_HC))
#define IS_SD_CARD(_card)           (((_card)->type == eSDC) || ((_card)->type == eSDC_V200) || ((_card)->type == eSDC_HC))
#define RELATIVE_CARD_ADDR(_sdmmc)  ((_sdmmc)->card.rca << 16)
#define NELEMS( _x )                ( sizeof( (_x) ) / sizeof( (_x)[0] ) )

/*
 * create a command structure
 */
#define CMD_CREATE(_sdmmc, _cr, _cmd, _arg, _rsp, _bsize, _bcnt, _dbuf, _flg) \
    do {                        \
        (_sdmmc)->cmd  = &(_cr);    \
        (_cr).cmd    = (_cmd);  \
        (_cr).arg    = (_arg);  \
        (_cr).rsp    = (_rsp);  \
        (_cr).bsize  = (_bsize);\
        (_cr).bcnt   = (_bcnt); \
        (_cr).dbuf   = (_dbuf); \
        (_cr).flgsts = (_flg);  \
    } while (0)

// command flags
#define SCF_CTYPE_BC        (1 << 0)
#define SCF_CTYPE_BCR       (1 << 1)
#define SCF_CTYPE_AC        (1 << 2)
#define SCF_CTYPE_ADTC      (1 << 3)

#define SCF_RSP_PRESENT     (1 << 4)
#define SCF_RSP_136         (1 << 5)    // 136 bit response
#define SCF_RSP_CRC         (1 << 6)    // expect valid crc
#define SCF_RSP_BUSY        (1 << 7)    // card may send busy
#define SCF_RSP_OPCODE      (1 << 8)    // response contains opcode

#define SCF_RSP_NONE        (0)
#define SCF_RSP_R1          (SCF_RSP_PRESENT | SCF_RSP_CRC | SCF_RSP_OPCODE)
#define SCF_RSP_R1B         (SCF_RSP_PRESENT | SCF_RSP_CRC | SCF_RSP_OPCODE | SCF_RSP_BUSY)
#define SCF_RSP_R2          (SCF_RSP_PRESENT | SCF_RSP_136 | SCF_RSP_CRC)
#define SCF_RSP_R3          (SCF_RSP_PRESENT)
#define SCF_RSP_R6          (SCF_RSP_PRESENT | SCF_RSP_CRC | SCF_RSP_OPCODE)
#define SCF_RSP_R7          (SCF_RSP_PRESENT | SCF_RSP_CRC | SCF_RSP_OPCODE)

#define SCF_DIR_IN          (1 << 9)    // data read
#define SCF_DIR_OUT         (1 << 10)   // data write
#define SCF_DATA_MSK        (SCF_DIR_IN | SCF_DIR_OUT)
#define SCF_APP_CMD         (1 << 11)   // app command (cmd 55)
#define SCF_SBC             (1 << 12)   // auto issue set block count (cmd 23)

#define SCF_MULTIBLK        (1 << 25)

/*
 * SD/MMC CID
 */
typedef struct _sdmmc_cid_t {
    uint32_t    mid;        // Manufacture ID
    uint32_t    oid;        // OEM/Application ID
    uint8_t     pnm[8];     // Product name
    uint32_t    prv;        // Product revision
    uint32_t    psn;        // Product serial number
    uint32_t    mdt;        // Manufacture date
} sdmmc_cid_t;

/*
 * SD/MMC CSD
 */
typedef struct _sdmmc_csd_t {
#define CSD_STRUCT_VER_10       0   // MMCA V1.0 / SD Standard Capacity
#define CSD_STRUCT_VER_20       1   // SD High Capacity / Extended Capacity
#define CSD_STRUCT_VER_30       2   // SD Ultra Capacity
#define CSD_STRUCT_VER_11       1   // MMCA V1.1
#define CSD_STRUCT_VER_12       2   // EMMC Version 4.1–4.2–4.3-4.41-4.5-4.51-5.0-5.01-5.1
    uint8_t     csd_structure;  // CSD structure

#define CSD_SPEC_VER_0          0   // 1.0 - 1.2 (MMC)
#define CSD_SPEC_VER_1          1   // 1.4 -
#define CSD_SPEC_VER_2          2   // 2.0 - 2.2
#define CSD_SPEC_VER_3          3   // 3.1 - 3.2 - 3.31
#define CSD_SPEC_VER_4          4   // 4.0 - 4.1
    uint8_t     spec_vers;
    uint8_t     tran_speed;
    uint8_t     read_bl_len;

#define CCC_BASIC       ( 1 << 0 )  // 0 Basic: CMD 0-10,12-15,19
#define CCC_BLOCK_READ  ( 1 << 2 )  // 2 Block Read: CMD 16-18
#define CCC_BLOCK_WRITE ( 1 << 4 )  // 4 Block Write: CMD 16,24-27
#define CCC_ERASE       ( 1 << 5 )  // 5 Erase:  CMD 32-39
#define CCC_WRITE_PROT  ( 1 << 6 )  // 6 Write Protection: CMD 28-31
#define CCC_LOCK_DEVICE ( 1 << 7 )  // 7 Lock Devcie: CMD 16,40,42
#define CCC_APP_SPEC    ( 1 << 8 )  // 8 Application specific: CMD 55-57, ACMD 6,13,22,23,41,42,51
#define CCC_IO_MODE     ( 1 << 9 )  // 9 (9) I/O mode: CMD 5,39,40,52,53
#define CCC_SWITCH      ( 1 << 10 ) // 10 High speed switch: CMD 6,34-37,50
    uint32_t    ccc;
    uint32_t    dtr_max;
    uint16_t    c_size;
    uint8_t     c_size_mult;
} sdmmc_csd_t;

/*
 * MMC Extend CSD
 */
typedef struct _mmc_ecsd_t {
#define ECSD_STROBE_SUPPORT     184
    #define ECSD_STROBE_SUPPORT_DISABLED    0   // HS400 (5.0)
    #define ECSD_STROBE_SUPPORT_ENABLED     1   // HS400ES (5.1)

#define ECSD_CARD_TYPE          196
    #define ECSD_CARD_TYPE_HS400_1_2V   (1<<7)  // Card can run at DDR 200MHz 1.2V
    #define ECSD_CARD_TYPE_HS400_1_8V   (1<<6)  // Card can run at DDR 200MHz 1.8V

    #define ECSD_CARD_TYPE_HS200_1_2V   (1<<5)  // Card can run at SDR 200MHz 1.2V
    #define ECSD_CARD_TYPE_HS200_1_8V   (1<<4)  // Card can run at SDR 200MHz 1.8V

    #define ECSD_CARD_TYPE_DDR_1_2V     (1<<3)  // Card can run at 52MHz 1.2V
    #define ECSD_CARD_TYPE_DDR_1_8V     (1<<2)  // Card can run at 52MHz 1.8V - 3.0V

    #define ECSD_CARD_TYPE_52           (1<<1)  // Card can run at 52MHz
    #define ECSD_CARD_TYPE_26           (1<<0)  // Card can run at 26MHz

    #define ECSD_CARD_TYPE_DDR          ( ECSD_CARD_TYPE_DDR_1_8V | ECSD_CARD_TYPE_DDR_1_2V )
    #define ECSD_CARD_TYPE_HS400        ( ECSD_CARD_TYPE_HS400_1_8V | ECSD_CARD_TYPE_HS400_1_2V )
    #define ECSD_CARD_TYPE_HS200        ( ECSD_CARD_TYPE_HS200_1_8V | ECSD_CARD_TYPE_HS200_1_2V )
    #define ECSD_CARD_TYPE_52MHZ        ( ECSD_CARD_TYPE_DDR_1_8V | ECSD_CARD_TYPE_DDR_1_2V | ECSD_CARD_TYPE_52 )
    #define ECSD_CARD_TYPE_MSK          0xff
    uint8_t     card_type;
    uint8_t     driver_strength;

#define ECSD_REV                192
    #define ECSD_REV_V5_1       8
    #define ECSD_REV_V5         7
    #define ECSD_REV_V4_5       6
    #define ECSD_REV_V4_41      5
    #define ECSD_REV_V4_4       4
    #define ECSD_REV_V4_3       3
    #define ECSD_REV_V4_2       2
    #define ECSD_REV_V4_1       1
    #define ECSD_REV_V4         0
    uint8_t     ext_csd_rev;
#define ECSD_SEC_CNT            212
    #define ECSD_SEC_CNT_2GB    0x400000
    uint8_t     part_cfg;

} mmc_ecsd_t;

/*
 * SDMMC card structure
 */
typedef struct card_s
{
    uint32_t    state;      // current state
    card_type_t type;       // card type
    uint32_t    blk_size;   // block size
    uint32_t    blk_num;    // number of blocks

    uint32_t    drv_type;

#define DEV_CAP_HC              (1 << 0)    // high capacity
#define DEV_CAP_HS              (1 << 1)    // high speed
#define DEV_CAP_HS200           (1 << 2)    // high speed 200
#define DEV_CAP_DDR50           (1 << 3)    // DDR
#define DEV_CAP_UHS             (1 << 4)    // UHS
#define DEV_CAP_TRIM            (1 << 5)    // TRIM supported
#define DEV_CAP_SECURE          (1 << 6)    // Secure Purge supported
#define DEV_CAP_SECURE_TRIM     (DEV_CAP_SECURE | DEV_CAP_TRIM)
#define DEV_CAP_SANITIZE        (1 << 7)    // SANITIZE supported
#define DEV_CAP_BKOPS           (1 << 8)    // Background Operations supported
#define DEV_CAP_CMD23           (1 << 9)    // CMD23 supported
#define DEV_CAP_SLEEP           (1 << 10)   // SLEEP/AWAKE supported
#define DEV_CAP_ASSD            (1 << 11)   // ASSD
#define DEV_CAP_HPI_CMD12       (1 << 12)
#define DEV_CAP_HPI_CMD13       (1 << 13)
#define DEV_CAP_DISCARD         (1 << 14)   // Discard supported
#define DEV_CAP_CACHE           (1 << 15)
#define DEV_CAP_HS400           (1 << 16)
#define DEV_CAP_PWROFF_NOTIFY   (1 << 17)   // Power off notify supported
#define DEV_CAP_HS400ES         (1 << 18)
#define DEV_CAP_BKOPS_AUTO      (1 << 19)   // Auto Background Operations supported
#define DEV_CAP_UC              (1 << 20)   // ultra capacity (2TB - 128TB)
    uint32_t    caps;
#define DEV_FLAG_PRESENT        0x001       // device present
#define DEV_FLAG_LOCKED         0x002
#define DEV_FLAG_INVALID_CARD   0x004
#define DEV_FLAG_MEDIA_CHANGE   0x008
#define DEV_FLAG_IDLE           0x010
#define DEV_FLAG_ACTIVE         0x020
#define DEV_FLAG_SLEEP          0x040
#define DEV_FLAG_HS             0x080       // high speed
#define DEV_FLAG_HS200          0x100       // high speed 200
#define DEV_FLAG_HS400          0x200       // high speed 400
#define DEV_FLAG_DDR            0x400       // DDR
#define DEV_FLAG_UHS            0x800       // UHS
#define DEV_FLAG_BKOPS          0x1000      // BKOPS
#define DEV_FLAG_SIG_ERR        0x2000      // signal switch error
#define DEV_FLAG_WRITE_PROTECT  0x4000      // write protected
#define DEV_FLAG_WCE            0x8000      // Write Cache Enable
#define DEV_FLAG_HS400ES        0x10000     // high speed 400 enhanced strobe
    uint32_t    flags;

    uint32_t    dtr_max_hs;
    uint16_t    rca;        // relative card address
} card_t;

typedef struct _sdmmc       sdmmc_t;
typedef struct _sdmmc_hc    sdmmc_hc_t;

/*
 * SDMMC host controller structure
 */
struct _sdmmc_hc
{
    paddr_t     sdmmc_pbase;        // Base address
    uint32_t    clock;              // SDHC clock
#define	HC_FLAG_DEV_SD              ( 1 << 4 )
#define	HC_FLAG_DEV_MMC             ( 1 << 5 )
    uint32_t    flags;
    void        (*set_clk)(sdmmc_t *, unsigned);
#define BUS_WIDTH_1             1
#define BUS_WIDTH_4             4
#define BUS_WIDTH_8             8
    void        (*set_bus_width)(sdmmc_t *, int);
#define BUS_MODE_OPEN_DRAIN     0
#define BUS_MODE_PUSH_PULL      1
    void        (*set_bus_mode)(sdmmc_t *, int);
#define TIMING_HS400ES          10
#define TIMING_HS400            9
#define TIMING_HS200            8
#define TIMING_SDR104           7
#define TIMING_SDR50            6
#define TIMING_SDR25            5
#define TIMING_SDR12            4
#define TIMING_DDR50            3
#define TIMING_HS               2
#define TIMING_LS               1
    void        (*set_timing)(sdmmc_t *, int);
    int         (*send_cmd)(sdmmc_t *);
#define SIGNAL_VOLTAGE_3_3      1
#define SIGNAL_VOLTAGE_3_0      2
#define SIGNAL_VOLTAGE_1_8      3
#define SIGNAL_VOLTAGE_1_2      4
    int         (*signal_voltage)(sdmmc_t *, int);
    int         (*tuning)(sdmmc_t *, int);
    int         (*power)(sdmmc_t *, int);
#define DRV_TYPE_B              1
#define DRV_TYPE_A              2
#define DRV_TYPE_C              4
#define DRV_TYPE_D              8
#define DRV_TYPE_MSK            0x0f
    int         (*drv_type)(sdmmc_t *, int type);
    int         (*driver_strength)(sdmmc_t *, int timing, int type);
    int         (*dinit_hc)(sdmmc_t *);
};

/*
 * SDMMC device structure
 */
struct _sdmmc
{
    blkdev_t    sdmmc;
    sdmmc_hc_t  *hc;

    uint32_t    ocr;                        // operation condition register

#define HC_CAP_HS                 (1 << 0)
#define HC_CAP_BUSY               (1 << 1)
#define HC_CAP_ACMD12             (1 << 2)
#define HC_CAP_ACMD23             (1 << 3)
#define HC_CAP_BW4                (1 << 4)
#define HC_CAP_BW8                (1 << 5)
#define HC_CAP_SDR12              (1 << 8)
#define HC_CAP_SDR25              (1 << 9)
#define HC_CAP_SDR50              (1 << 10)
#define HC_CAP_SDR104             (1 << 11)
#define HC_CAP_DDR50              (1 << 12)
#define HC_CAP_UHS(_caps)         ((_caps) & (HC_CAP_SDR12 | HC_CAP_SDR25 | \
                                            HC_CAP_SDR50 | HC_CAP_SDR104 | HC_CAP_DDR50))

#define HC_CAP_HS200              (1 << 13)
#define HC_CAP_HS400              (1 << 14)
#define HC_CAP_HS400ES            (1 << 15)

#define HC_CAP_SV_1_2V            (1 << 16)   // 1.2V signal voltage supported
#define HC_CAP_SV_1_8V            (1 << 17)   // 1.8V signal voltage supported
#define HC_CAP_SV_3_0V            (1 << 18)   // 3.0V signal voltage supported
#define HC_CAP_SV_3_3V            (1 << 19)   // 3.3V signal voltage supported
#define HC_CAP_SV(_caps)          ((((_caps) >> 16)) & 0x0f)

#define HC_CAP_XPC_3_3V           (1 << 20)   // > 150mA at 3.3V is supported
#define HC_CAP_XPC_3_0V           (1 << 21)   // > 150mA at 3.0V is supported
#define HC_CAP_XPC_1_8V           (1 << 22)   // > 150mA at 1.8V is supported
#define HC_CAP_XPC(_caps)         (((_caps) >> 20) & 0x07)

#define	HC_CAP_200MA              (1 << 23)	// 200mA at 1.8V
#define	HC_CAP_400MA              (1 << 24)	// 400mA at 1.8V
#define	HC_CAP_600MA              (1 << 25)	// 600mA at 1.8V
#define	HC_CAP_800MA              (1 << 26)	// 800mA at 1.8V
#define HC_CAP_CURRENT( _caps )   ((((_caps) >> 23)) & 0x0f)

#define	HC_CAP_DRV_TYPE_B         (1 << 27)
#define	HC_CAP_DRV_TYPE_A         (1 << 28)
#define	HC_CAP_DRV_TYPE_C         (1 << 29)
#define	HC_CAP_DRV_TYPE_D         (1 << 30)
#define	HC_CAP_DRV_TYPES( _caps ) ((((_caps ) >> 27)) & 0x0f)

#define HC_CAP_SKIP_IDENT         (1 << 31)
    uint32_t    caps;

    sdmmc_cmd_t *cmd;           // command request structure
    card_t      card;           // card structure

    sdmmc_cid_t cid;            // CID
    sdmmc_csd_t csd;            // CSD

    int         timing;
    int         signal_voltage;
    int         bus_width;

#define SDMMC_VERBOSE_LVL_0     0   // Error only
#define SDMMC_VERBOSE_LVL_1     1   // Warnings and some info
#define SDMMC_VERBOSE_LVL_2     2   // More info, such as clock, bus width, etc.
#define SDMMC_VERBOSE_LVL_3     3   // Command details
    int         verbose;
};

#define POWER_UP_WAIT               40000

extern const uint8_t        sdio_tbp_4bit[64];
extern const uint8_t        sdio_tbp_8bit[128];


typedef struct _sdmmc_ext_t {
    uint32_t    chip_type;
    uint32_t    chip_rev;
    uint32_t    tuning_flags;
    uint32_t    dev_flags;
    uint32_t    drv_type;
    uint32_t    rvsd[4];
} sdmmc_ext_t;

/*
* SDMM functions
*/
extern int sdmmc_init_sd(sdmmc_t *sdmmc);
extern int sdmmc_init_mmc(sdmmc_t *sdmmc);

extern int sdmmc_go_idle(sdmmc_t *sdmmc);
extern int sdmmc_get_state(sdmmc_t *sdmmc);
extern int sdmmc_wait_card_state(sdmmc_t *sdmmc, uint32_t mask, uint32_t val, uint32_t msec);
extern int sdmmc_select_voltage(sdmmc_t *sdmmc, uint32_t ocr);
extern int sdmmc_send_csd(sdmmc_t *sdmmc, uint32_t *csd);
extern int sdmmc_all_send_cid(sdmmc_t *sdmmc, uint32_t *cid);
extern int sdmmc_select_card(sdmmc_t *sdmmc);
extern int sdmmc_set_block_length(sdmmc_t *sdmmc, int blklen);
extern int sdmmc_read_write(void *ext, void *buf, uint32_t blkno, uint32_t blkcnt, int rd);
extern unsigned sdmmc_get_mmc_boot_partition(sdmmc_t *sdmmc);
extern int sdmmc_set_mmc_partition(sdmmc_t *sdmmc, unsigned partition);

extern void imx8_init_hc(sdmmc_t *sdmmc, const paddr_t base, const uint32_t clock, const int verbose,
                            const sdmmc_ext_t *ext);

#endif /* _SDMMC_H_INCLUDE */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/sdmmc/sdmmc.h $ $Rev: 980326 $")
#endif
