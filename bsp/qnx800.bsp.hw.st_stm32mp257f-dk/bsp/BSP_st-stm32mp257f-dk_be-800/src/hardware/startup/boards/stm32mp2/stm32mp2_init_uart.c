/*
 * Copyright 2026 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
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

/*
 * Polled serial operations for STM32MP2
 */

#include <startup.h>

#include "board.h"
#include <soc/st/stm32mp2/include/stm32_uart.h>
#include <soc/st/stm32mp2/include/stm32mp25_rcc.h>

/**
 * STM32MP257F-DK startup source file.
 *
 * @file       stm32mp2_init_uart.c
 * @addtogroup startup
 * @{
 */

#define STM32_UART_CR1_FIELDS \
        (STM32_USART_CR1_M | STM32_USART_CR1_PCE | STM32_USART_CR1_PS | STM32_USART_CR1_TE | \
         STM32_USART_CR1_RE | STM32_USART_CR1_OVER8 | STM32_USART_CR1_FIFOEN)

#define STM32_UART_CR2_FIELDS \
        (STM32_USART_CR2_SLVEN | STM32_USART_CR2_DIS_NSS | STM32_USART_CR2_ADDM7 | \
         STM32_USART_CR2_LBDL | STM32_USART_CR2_LBDIE | STM32_USART_CR2_LBCL | \
         STM32_USART_CR2_CPHA | STM32_USART_CR2_CPOL | STM32_USART_CR2_CLKEN | \
         STM32_USART_CR2_STOP | STM32_USART_CR2_LINEN | STM32_USART_CR2_SWAP | \
         STM32_USART_CR2_RXINV | STM32_USART_CR2_TXINV | STM32_USART_CR2_DATAINV | \
         STM32_USART_CR2_MSBFIRST | STM32_USART_CR2_ABREN | STM32_USART_CR2_ABRMODE | \
         STM32_USART_CR2_RTOEN | STM32_USART_CR2_ADD)

#define STM32_UART_CR3_FIELDS \
        (STM32_USART_CR3_EIE | STM32_USART_CR3_IREN | STM32_USART_CR3_IRLP | \
         STM32_USART_CR3_HDSEL | STM32_USART_CR3_NACK | STM32_USART_CR3_SCEN | \
         STM32_USART_CR3_DMAR | STM32_USART_CR3_DMAT | STM32_USART_CR3_RTSE | \
         STM32_USART_CR3_CTSE | STM32_USART_CR3_CTSIE | STM32_USART_CR3_ONEBIT | \
         STM32_USART_CR3_OVRDIS | STM32_USART_CR3_DDRE | STM32_USART_CR3_DEM | \
         STM32_USART_CR3_DEP | STM32_USART_CR3_SCARCNT | STM32_USART_CR3_WUS | \
         STM32_USART_CR3_WUFIE | STM32_USART_CR3_TXFTIE | STM32_USART_CR3_TCBGTIE | \
         STM32_USART_CR3_RXFTCFG | STM32_USART_CR3_RXFTIE | STM32_USART_CR3_TXFTCFG)

#define STM32_UART_ISR_ERRORS     \
        (STM32_USART_ISR_ORE | STM32_USART_ISR_NE |  STM32_USART_ISR_FE | STM32_USART_ISR_PE)

struct stm32_uart_init_s {
    uint32_t baud_rate;        /*
                     * Configures the UART communication
                     * baud rate.
                     */

    uint32_t word_length;        /*
                     * Specifies the number of data bits
                     * transmitted or received in a frame.
                     * This parameter can be a value of
                     * @ref STM32_UART_WORDLENGTH_*.
                     */

    uint32_t stop_bits;        /*
                     * Specifies the number of stop bits
                     * transmitted. This parameter can be
                     * a value of @ref STM32_UART_STOPBITS_*.
                     */

    uint32_t parity;        /*
                     * Specifies the parity mode.
                     * This parameter can be a value of
                     * @ref STM32_UART_PARITY_*.
                     */

    uint32_t mode;            /*
                     * Specifies whether the receive or
                     * transmit mode is enabled or
                     * disabled. This parameter can be a
                     * value of @ref @ref STM32_UART_MODE_*.
                     */

    uint32_t hw_flow_control;    /*
                     * Specifies whether the hardware flow
                     * control mode is enabled or
                     * disabled. This parameter can be a
                     * value of @ref STM32_UARTHWCONTROL_*.
                     */

    uint32_t one_bit_sampling;    /*
                     * Specifies whether a single sample
                     * or three samples' majority vote is
                     * selected. This parameter can be 0
                     * or STM32_USART_CR3_ONEBIT.
                     */

    uint32_t prescaler;        /*
                     * Specifies the prescaler value used
                     * to divide the UART clock source.
                     * This parameter can be a value of
                     * @ref STM32_UART_PRESCALER_*.
                     */

    uint32_t fifo_mode;        /*
                     * Specifies if the FIFO mode will be
                     * used. This parameter can be a value
                     * of @ref STM32_UART_FIFOMODE_*.
                     */

    uint32_t tx_fifo_threshold;    /*
                     * Specifies the TXFIFO threshold
                     * level. This parameter can be a
                     * value of @ref
                     * STM32_UART_TXFIFO_THRESHOLD_*.
                     */

    uint32_t rx_fifo_threshold;    /*
                     * Specifies the RXFIFO threshold
                     * level. This parameter can be a
                     * value of @ref
                     * STM32_UART_RXFIFO_THRESHOLD_*.
                     */
};

struct stm32_uart_handle_s {
    uint32_t base;
    uint32_t rdr_mask;
    unsigned clockfreq;
};

static const uint16_t presc_table[STM32_UART_PRESCALER_NB] = {
    1U, 2U, 4U, 6U, 8U, 10U, 12U, 16U, 32U, 64U, 128U, 256U
};

/**
 * Parse UART initialization options (base address, input clock, baudrate).
 *
 * @param channel Debug device index (in debug_devices structure)
 * @param line    String line to parse.
 * @param baud    Pointer to baudrate variable.
 * @param clk     Pointer to input peripheral clock variable.
 */
static void parse_line(unsigned channel, const char *line, unsigned *baud, unsigned *clk)
{
    /* Get device base address and register stride */
    if ((*line != '.') && (*line != '\0')) {
        dbg_device[channel].base = strtoul(line, (char **)&line, 16);
        if (*line == '^') {
            dbg_device[channel].shift = strtoul(line + 1, (char **)&line, 0);
        }
    }

    /* Get baud rate value */
    if (*line == '.') {
        ++line;
    }
    if ((*line != '.') && (*line != '\0')) {
        *baud = strtoul(line, (char **)&line, 0);
    }

    /* Get input device clock rate value */
    if (*line == '.') {
        ++line;
    }
    if (*line != '.' && *line != '\0') {
        *clk = strtoul(line, (char **)&line, 0);
    }
}

/*
 * @brief  Compute RDR register mask depending on word length.
 * @param  word_length: Word length of the UART.
 * @retval Mask value.
 */
static unsigned int stm32_uart_rdr_mask(const struct stm32_uart_init_s *init)
{
    unsigned int mask = 0U;

    switch (init->word_length) {
    case STM32_UART_WORDLENGTH_9B:
        mask = GENMASK(8, 0);
        break;
    case STM32_UART_WORDLENGTH_8B:
        mask = GENMASK(7, 0);
        break;
    case STM32_UART_WORDLENGTH_7B:
        mask = GENMASK(6, 0);
        break;
    default:
        break; /* not reached */
    }

    if (init->parity != STM32_UART_PARITY_NONE) {
        mask >>= 1;
    }

    return mask;
}

/* @brief  BRR division operation to set BRR register in 8-bit oversampling
 * mode.
 * @param  clockfreq: UART clock.
 * @param  baud_rate: Baud rate set by the user.
 * @param  prescaler: UART prescaler value.
 * @retval Division result.
 */
static uint32_t uart_div_sampling8(unsigned long clockfreq,
                                   uint32_t baud_rate,
                                   uint32_t prescaler)
{
    uint32_t scaled_freq = clockfreq / presc_table[prescaler];

    return ((scaled_freq * 2) + (baud_rate / 2)) / baud_rate;

}

/* @brief  BRR division operation to set BRR register in 16-bit oversampling
 * mode.
 * @param  clockfreq: UART clock.
 * @param  baud_rate: Baud rate set by the user.
 * @param  prescaler: UART prescaler value.
 * @retval Division result.
 */
static uint32_t uart_div_sampling16(unsigned long clockfreq,
                                    uint32_t baud_rate,
                                    uint32_t prescaler)
{
    uint32_t scaled_freq = clockfreq / presc_table[prescaler];

    return (scaled_freq + (baud_rate / 2)) / baud_rate;

}

/*
 * @brief  Configure the UART peripheral.
 * @param  huart: UART handle.
 * @retval UART status.
 */
static int uart_set_config(struct stm32_uart_handle_s *huart,
                           const struct stm32_uart_init_s *init)
{
    uint32_t tmpreg, reg_val;
    unsigned long int_div;
    uint32_t brrtemp;
    uint32_t over_sampling;

    /*---------------------- USART BRR configuration --------------------*/
    int_div = huart->clockfreq / init->baud_rate;
    if (int_div < 16U) {
        uint32_t usartdiv = uart_div_sampling8(huart->clockfreq,
                                               init->baud_rate,
                                               init->prescaler);

        brrtemp = (usartdiv & STM32_USART_BRR_DIV_MANTISSA) |
              ((usartdiv & STM32_USART_BRR_DIV_FRACTION) >> 1);
        over_sampling = STM32_USART_CR1_OVER8;
    } else {
        brrtemp = uart_div_sampling16(huart->clockfreq,
                                      init->baud_rate,
                                      init->prescaler) &
              (STM32_USART_BRR_DIV_FRACTION | STM32_USART_BRR_DIV_MANTISSA);
        over_sampling = 0x0U;
    }
    out32(huart->base + STM32_USART_BRR, brrtemp);

    /*
     * ---------------------- USART CR1 Configuration --------------------
     * Clear M, PCE, PS, TE, RE and OVER8 bits and configure
     * the UART word length, parity, mode and oversampling:
     * - set the M bits according to init->word_length value,
     * - set PCE and PS bits according to init->parity value,
     * - set TE and RE bits according to init->mode value,
     * - set OVER8 bit according baudrate and clock.
     */
    tmpreg = init->word_length |
         init->parity |
         init->mode |
         over_sampling |
         init->fifo_mode;
    reg_val = in32(huart->base + STM32_USART_CR1);
    reg_val &= ~STM32_UART_CR1_FIELDS;
    reg_val |= tmpreg;
    // out32(huart->base + STM32_USART_CR1, reg_val);

    /*
     * --------------------- USART CR2 Configuration ---------------------
     * Configure the UART Stop Bits: Set STOP[13:12] bits according
     * to init->stop_bits value.
     */
    reg_val = in32(huart->base + STM32_USART_CR2);
    reg_val &= ~STM32_UART_CR2_FIELDS;
    reg_val |= init->stop_bits;
    out32(huart->base + STM32_USART_CR2, reg_val);

    /*
     * --------------------- USART CR3 Configuration ---------------------
     * Configure:
     * - UART HardWare Flow Control: set CTSE and RTSE bits according
     *   to init->hw_flow_control value,
     * - one-bit sampling method versus three samples' majority rule
     *   according to init->one_bit_sampling (not applicable to
     *   LPUART),
     * - set TXFTCFG bit according to init->tx_fifo_threshold value,
     * - set RXFTCFG bit according to init->rx_fifo_threshold value.
     */
    tmpreg = init->hw_flow_control | init->one_bit_sampling;

    if (init->fifo_mode == STM32_USART_CR1_FIFOEN) {
        tmpreg |= init->tx_fifo_threshold |
              init->rx_fifo_threshold;
    }

    reg_val = in32(huart->base + STM32_USART_CR3);
    reg_val &= ~STM32_UART_CR3_FIELDS;
    reg_val |= tmpreg;
    out32(huart->base + STM32_USART_CR3, reg_val);

    /*
     * --------------------- USART PRESC Configuration -------------------
     * Configure UART Clock Prescaler : set PRESCALER according to
     * init->prescaler value.
     */
    if(init->prescaler >= STM32_UART_PRESCALER_NB) {
        crash("uart wrong prescaler %d! Status: 0x%x. \n", init->prescaler, -1);
        return -1;
    }

    reg_val = in32(huart->base + STM32_USART_PRESC);
    reg_val &= ~STM32_USART_PRESC_PRESCALER;
    reg_val |= init->prescaler;
    out32(huart->base + STM32_USART_PRESC, reg_val);

    return 0;
}


/**
 * Initialise one of the serial ports.
 *
 * @param channel   Debug device index (in debug_devices structure)
 * @param init      String line with configuration parameters.
 * @param defaults  String line with configuration parameters.
 */
void stm32mp2_init_uart(unsigned channel, const char *init, const char *defaults)
{
    int status = 0;
    uint32_t    val;
    struct stm32_uart_handle_s huart;
    struct stm32_uart_init_s uart_cfg;

    huart.clockfreq = 64000000;  /* default peripheral clock rate is 64MHz */

    /*
     * Default UART settings:
     * Baud rate: 115200
     * Data: 8 bit
     * Parity: none
     * Stop: 1 bit
     * Flow control: none
     */
    uart_cfg.baud_rate = 115200;
    uart_cfg.word_length = STM32_UART_WORDLENGTH_8B;
    uart_cfg.stop_bits = STM32_UART_STOPBITS_1;
    uart_cfg.parity = STM32_UART_PARITY_NONE;
    uart_cfg.prescaler = STM32_UART_PRESCALER_DIV1;
    uart_cfg.mode = STM32_UART_MODE_TX_RX;
    uart_cfg.fifo_mode = STM32_UART_FIFOMODE_DIS;
    uart_cfg.one_bit_sampling = STM32_USART_CR3_ONEBIT;
    uart_cfg.hw_flow_control = STM32_UART_HWCONTROL_NONE;

    parse_line(channel, defaults, &uart_cfg.baud_rate, &huart.clockfreq);
    parse_line(channel, init, &uart_cfg.baud_rate, &huart.clockfreq);
    huart.base = dbg_device[channel].base;

    if (uart_cfg.baud_rate == 0) {
        crash("uart wrong baud rate %d! Status: 0x%x. \n", uart_cfg.baud_rate, status);
        return;
    }

    /* Enable USART2 clock */
    val = in32(STM32MP2_RCC_BASE_ADDR + RCC_USART2CFGR);
    val |= RCC_USART2CFGR_USART2EN;
    out32(STM32MP2_RCC_BASE_ADDR + RCC_USART2CFGR, val);
    while (!(in32(STM32MP2_RCC_BASE_ADDR + RCC_USART2CFGR) & RCC_USART2CFGR_USART2EN)) {}

    /* Disable UART */
    val = in32(huart.base + STM32_USART_CR1);
    val &= ~STM32_USART_CR1_UE;
    out32(huart.base + STM32_USART_CR1, val);

    /* Computation of UART mask to apply to RDR register */
    huart.rdr_mask = stm32_uart_rdr_mask(&uart_cfg);

    /* Initialize UART configuration */
    status = uart_set_config(&huart, &uart_cfg);
    if (status != 0) {
        crash("uart initialization failed! Status: 0x%x. \n", status);
        return;
    }

    /* Enable UART */
    val = in32(huart.base + STM32_USART_CR1);
    val |= STM32_USART_CR1_UE;
    out32(huart.base + STM32_USART_CR1, val);
}

/**
 * Send a character.
 *
 * @param data Character to send.
 */
void stm32mp2_uart_put_char(int data)
{
    unsigned base = dbg_device[0].base;

    while (!(in32(base + STM32_USART_ISR) & STM32_USART_ISR_TXE)) {}
    out32(base + STM32_USART_TDR, (unsigned)data);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/stm32mp2_init_uart.c $ $Rev: 984580 $")
#endif
