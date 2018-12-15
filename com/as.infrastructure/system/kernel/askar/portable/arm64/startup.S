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
;;
_start:

reset:
	ldr x30, =stack_top
	mov sp, x30
	ldr x0, =vector_table_el1
	msr vbar_el1, x0
	b main
	b .

	.section .text
	.global Irq_Restore
	.type   Irq_Restore, %function
/* void Irq_Restore( imask_t intsts ); */
Irq_Restore:
	msr daif, x0
	ret

	.global __Irq_Save
	.type   __Irq_Save, %function
/* imask_t __Irq_Save( void ); */
__Irq_Save:
	mrs x0, daif
	msr daifset, #0b0010
	ret

/* void Irq_Disable(void)   */
	.global Irq_Disable
	.type   Irq_Disable, %function
Irq_Disable:
	msr daifset, #0b0010
	ret

/* void Irq_Enable(void) */
	.global Irq_Enable
	.type   Irq_Enable, %function
Irq_Enable:
	msr daifclr, #0b0010
	ret

/* int Os_PortGetCurrentEL(void) */
	.global Os_PortGetCurrentEL
	.type   Os_PortGetCurrentEL, %function
Os_PortGetCurrentEL:
	mrs x0, currentel
	ret

