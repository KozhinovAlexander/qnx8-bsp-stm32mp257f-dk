/*
 * Copyright (c) 2007, 2008, 2022-2023, BlackBerry Limited.
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
#include <hw/inout.h>
#include <hw/8250.h>


static char ser8250_getchar(void);
static char ser8250_pollkey(void);
static void ser8250_putchar(const char data);

//
//  user defined defaults
//

#define DEFAULT_CLK             1843200
#define DEFAULT_DIV             16

static paddr_t  port_base;
static uint8_t  port_size;
static uint8_t  reg_shift;


static void ser8250_writeport(const paddr_t address, const uint32_t data)
{
    switch (port_size) {
        case 4:
            out32(address, data);
            break;
        case 2:
            out16(address, (uint16_t)data);
            break;
        default:
            out8(address, (uint8_t)data);
            break;
    }
}

static uint32_t ser8250_readport(const paddr_t address)
{
    switch (port_size) {
        case 4:
            return (in32(address));
        case 2:
            return ((uint32_t)in16(address));
        default:
            return ((uint32_t)in8(address));
    }
}

static void ser8250_setport(const paddr_t address, const uint8_t mask, const uint8_t data)
{
    uint8_t c;

    c = (uint8_t)ser8250_readport(address);
    ser8250_writeport(address, (uint32_t)((c & ~mask) | (data & mask)));
}

void
init_ser8250(const paddr_t address, const uint8_t size, const uint8_t shift, const uint32_t baud, uint32_t clk, uint32_t divisor)
{
    uint32_t brd = 0;
    static const ser_dev ser8250_dev = {
        .get_byte = ser8250_getchar,
        .put_byte = ser8250_putchar,
        .poll     = ser8250_pollkey
    };
    /*
     * Initialize port base, size and shift count
     */
    port_base = address;
    reg_shift = shift;
    port_size = size;

    /*
     * This routine will initialize the selected 8250 serial port
     * to 8N1 parameters.
     */

    if (baud != 0) {
        /*
         * Set Baud rate
         */
        /* brd = clk / (baud * divisor); */
        for (divisor *= baud; clk >= divisor; clk -= divisor) {
            ++brd;
        }
        if ((clk << 1) >= divisor) {
            ++brd;
        }

        ser8250_setport(address + (paddr_t)(REG_LC << shift), LCR_DLAB, LCR_DLAB);
        ser8250_setport(address + (paddr_t)(REG_DL0 << shift), 0xFF, (uint8_t)(brd & 0xFF));
        ser8250_setport(address + (paddr_t)(REG_DL1 << shift), 0xFF, (uint8_t)(brd >> 8));
        ser8250_setport(address + (paddr_t)(REG_LC << shift), LCR_DLAB, 0);

        /*
         * Set data bits to 8
         */
        ser8250_setport(address + (paddr_t)(REG_LC << shift), 0xFF, 0x03);
    }

    /*
     * Register our debug functions
     */
    init_serdev((ser_dev *)&ser8250_dev);
}

static char ser8250_pollkey(void)
{
    return (ser8250_readport(port_base + (paddr_t)(REG_LS << reg_shift)) & LSR_RXRDY) ? 1 : 0;
}

static char ser8250_getchar(void)
{
    /*
     *  wait for data to be available
     */
    while (!ser8250_pollkey()) {

    }

    return (ser8250_readport(port_base + (paddr_t)(REG_RX << reg_shift)));
}

static void ser8250_putchar(const char data)
{
    /*
     *  wait for transmitter ready
     */
    while (!(ser8250_readport(port_base + (paddr_t)(REG_LS << reg_shift)) & LSR_TXRDY)) {
    }

    ser8250_writeport(port_base, (uint32_t)data);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/ser8250.c $ $Rev: 975396 $")
#endif
