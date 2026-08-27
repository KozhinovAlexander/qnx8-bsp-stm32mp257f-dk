/*
 * Copyright (c) 2017, 2022-2023, BlackBerry Limited.
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




	.text
	.align	2

jump:
	mov		x30, x0

	/* zero our argument registers */
	mov		x0, #0
	mov		x1, #0
	mov		x2, #0
	/*
	* FDT address is loaded into x3 during image start,
	* copy the address to x0 to make it consistent with u-boot
	*/
	mov		x0, x3
	/* clear x3 once the FDT address is copied to x0 */
	mov		x3, #0

	br		x30

	.globl	jump
	.size	jump,.-jump
	.type	jump,function

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/jump.S $ $Rev: 975396 $";
.previous
#endif
#endif
