/*
 * $QNXLicenseC:
 * Copyright 2016, 2022, 2023 BlackBerry Limited.
 * Copyright 2019, 2022 NXP
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


#ifndef IMX_STARTUP_H_
#define IMX_STARTUP_H_

#include <soc/nxp/imx8/mp/mx8mp.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_startup.h
 * @addtogroup startup
 * @{
 */

typedef uint64_t   imx_base_t;
#define IMX_IN     in32
#define IMX_OUT    out32

/** i.mx chip types list */
#define IMX_iMX8QM_MCU_TYPE         0x01
#define IMX_iMX8QXP_MCU_TYPE        0x02
#define IMX_iMX8MM_MCU_TYPE         0x80
#define IMX_iMX8MML_MCU_TYPE        0x81
#define IMX_iMX8MMD_MCU_TYPE        0x82
#define IMX_iMX8MMDL_MCU_TYPE       0x83
#define IMX_iMX8MMS_MCU_TYPE        0x84
#define IMX_iMX8MMSL_MCU_TYPE       0x85
#define IMX_iMX8MQ_MCU_TYPE         0x90
#define IMX_iMX8MQL_MCU_TYPE        0x91
#define IMX_iMX8MD_MCU_TYPE         0x92
#define IMX_IMX8MP_MCU_TYPE         0xA0
#define IMX_IMX8MP6_MCU_TYPE        0xA1
#define IMX_IMX8MPL_MCU_TYPE        0xA2
#define IMX_IMX8MPD_MCU_TYPE        0xA3
#define IMX_IMX8MPUL_MCU_TYPE       0xA4

#define IMX_MCU_TYPE_MASK           0xF0
#define IMX_MCU_VARIANT_MASK        0x0F


/** Startup global data structure */
typedef struct {
    uint32_t    chip_rev;                           /**< Processor revision */
    uint32_t    chip_type;                          /**< Processor type */
    uint32_t    imx_uart_clock[IMX_UART_COUNT];     /**< Array of UART clocks used in HWI */
    uint32_t    imx_usdhc_clk[IMX_USDHC_COUNT];     /**< Array of USDHC clocks used in HWI */
} imx_startup_data_t;

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

/* Startup command line arguments */
#define IMX_WDOG_ENABLE            (1 << 0)

extern void imx_init_uart(unsigned channel, const char *init, const char *defaults);
extern void imx_uart_put_char(int);

extern struct callout_rtn imx_uart_display_char;
extern struct callout_rtn imx_uart_poll_key;
extern struct callout_rtn imx_uart_break_detect;

extern unsigned char soc_overdrive;

void imx_timer_init(void);
unsigned int imx_get_timer_val(void);
unsigned int imx_get_timer_delta(unsigned int t_first, unsigned int t_second);
void imx_timer_print_delta(unsigned int t_first, unsigned int t_second);
void imx_usleep(uint32_t sleep_duration);
void imx_init_raminfo(void);
void init_qtime(void);
int imx_init_clocks(imx_startup_data_t *startup_data);
int imx_init_pads(imx_startup_data_t *startup_data);
void imx_dump_clocks(imx_startup_data_t *startup_data);
void imx_init_usdhc_clk(void);
uint32_t imx_get_cpu_clk(void);
void imx_wdg_enable(void);
void imx_init_usb_otg1(void);
void imx_init_usb3_otg2(void);
void imx_init_usb_host1(void);
void imx_board_mmu_enable(unsigned base, unsigned size);

uint32_t imx_set_gpio_output(uint32_t base, uint32_t pin, uint32_t level);
uint32_t imx_set_gpio_input(uint32_t base, uint32_t pin);
uint32_t imx_reset_gpio_pin(uint32_t base, uint32_t pin,  uint32_t level);
uint32_t imx_reset_gpio_pin_fin(uint32_t dur);
uint32_t imx_get_gpio_value(uint32_t base, uint32_t pin, uint32_t *gpio_val);
uint32_t imx_gpio_set_irq_mode(uint32_t base, uint32_t pin, uint32_t irq_mode);
uint32_t imx_get_chip_rev(void);
uint32_t imx_get_chip_type(void);
void print_chip_info(void);


#endif /* IMX_STARTUP_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_startup.h $ $Rev: 979659 $")
#endif
