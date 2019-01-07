/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
.extern __bss_start
.extern __bss_end
.extern main
.extern vector_table_el1
.extern stack_top

	.section .init

	.globl _start
_start:
	/* set up stack */
	mov x4, #1
	msr spsel, x4
	isb
	ldr x4, =stack_top
	mov sp, x4

	/* enable FP/ASIMD */
	mov x4, #(3 << 20)
	msr cpacr_el1, x4

	/* set up exception handling */
	ldr x4, =vector_table_el1
	msr vbar_el1, x4
	isb

	b main
	b .
