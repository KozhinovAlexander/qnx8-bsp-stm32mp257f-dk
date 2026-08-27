/*
 * Copyright (c) 2022, BlackBerry Limited.
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
 */

/*
 * Renesas RZ/G2L processor with ARMv8
 */

#ifndef	__AARCH64_RZG2L_H_INCLUDED
#define	__AARCH64_RZG2L_H_INCLUDED

/*
 * System Memory Map
 */

/*
 * SCIFA
 */
#define RZG2L_SCIFA0_BASE		0x1004B800
#define RZG2L_SCIFA1_BASE		0x1004BC00
#define RZG2L_SCIFA2_BASE		0x1004C000
#define RZG2L_SCIFA3_BASE		0x1004C400
#define RZG2L_SCIFA4_BASE		0x1004C800
#define RZG2L_SCIFA0_INT		412
#define RZG2L_SCIFA1_INT		417
#define RZG2L_SCIFA2_INT		422
#define RZG2L_SCIFA3_INT		427
#define RZG2L_SCIFA4_INT		432

/*
 * Watchdog Timer
 */
#define RZG2L_WDT0_BASE                0x12800800	// WDT CA55 core 0
#define RZG2L_WDT1_BASE                0x12800C00	// WDT CA55 core 1
#define RZG2L_WDT_SIZE                 0x100		// WDT CA55 core 1

#define WDT_CNT                0x00		// WDT Count Register
#define WDT_SET                0x04		// WDT Period Setting Register
#define WDT_TIM                0x08		// WDT Elapsed Time Register
#define WDT_INT                0x0C		// WDT Interrupt Control register
#define WDT_PECR               0x10		// Parity Error Control register
#define WDT_PEEN               0x14		// Parity Error Force Enable register
#define WDT_PESR               0x18		// Parity Status register
#define WDT_PEER               0x1C		// Parity Error Enable register
#define WDT_CNT_EN             (1 << 0)	// WDT enable
#define DT_INT_INTDISP         (1 << 0)	// WDT interrupt status read and clear
#define WDT_PEEN_FORCE_RST     (1 << 0)	// WDT force reset
#define WDT_SET_MASK           (0xFFF00000)	// WDT_SET MASK

/*
 * CPG
 */

/* Domain base address */
#define RZG2L_CPG_BASE			0x11010000
#define RZG2L_CPG_SIZE			0x1000

#define CPG_PL2_SDHI_DSEL	0x218
#define CPG_CLK_STATUS		0x280
#define CPG_WDTRST_SEL		0xB14
#define CPG_WDTRST_SEL_0	(1 << 0)


#define CPG_CLKON_BASE			(RZG2L_CPG_BASE + 0x500)
#define CPG_RESET_BASE			(RZG2L_CPG_BASE + 0x800)
#define CPG_RESET_ETH		0x7C
#define CPG_RESET_I2C		0x80


/*
 * GIC-600
 */
#define RZG2L_GICD_BASE                0x11900000
#define RZG2L_GICA_BASE                0x11910000
#define RZG2L_GICT_BASE                0x11920000
#define RZG2L_GICP_BASE                0x11930000
#define RZG2L_GICR_LPI_BASE0           0x11940000	//Core 0
#define RZG2L_GICR_SGI_BASE0           0x11950000	//Core 0
#define RZG2L_GICR_LPI_BASE1           0x11960000	//Core 1
#define RZG2L_GICR_SGI_BASE1           0x11970000	//Core 1


#endif	/* __AARCH64_RZG2L_H_INCLUDED */
