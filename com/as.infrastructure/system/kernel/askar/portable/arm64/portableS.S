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

/* https://static.docs.arm.com/100933/0100/aarch64_exception_and_interrupt_handling_100933_0100_en.pdf
 * https://www.arm.com/files/downloads/ARMv8_Architecture.pdf
 * http://infocenter.arm.com/help/topic/com.arm.doc.uan0015a/cortex_a57_software_optimisation_guide_external.pdf
 * https://developer.arm.com/docs/ddi0596/a/a64-base-instructions-alphabetic-order
 *  X30 -> LR, X29 -> BSP
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#define MACROS_ONLY
#include "Os_Cfg.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
	.extern Irq_Enable
	.extern Irq_Disable
	.extern Os_PortIsrHandler
	.extern Sched_Preempt
/* ============================ [ DATAS     ] ====================================================== */
	.extern RunningVar
	.extern ReadyVar
	.extern CallLevel
	.extern ISR2Counter
	.extern stack_top
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
	.section .text

Os_PortIdle:
	/* set RunningVar to NULL */
	ldr     x1, =RunningVar
	str     x0, [x1]
	bl      Irq_Enable
l_idle:
	ldr     x0, =ReadyVar
	ldr     x0, [x0]
	cmp     x0, #0
	ble     l_idle
	bl      Irq_Disable
	bl      Sched_GetReady
	b       Os_PortStartDispatch

	.global Os_PortStartDispatch
	.type   Os_PortStartDispatch, %function
/* void Os_PortStartDispatch( void ); */
Os_PortStartDispatch:
	ldr     x0, =ReadyVar
	ldr     x0, [x0]
	cmp     x0, #0
	beq     Os_PortIdle
	ldr     x1, =RunningVar
	str     x0, [x1]
Os_PortDispatchFormISR:
	#ifdef OS_USE_PRETASK_HOOK
	ldr     x1, = CallLevel
	ldr     w3, [x1]
	mov     w2, #8   /* CallLevel = TCL_PREPOST */
	str     w2,[x1]
	stp     x0,x1, [sp, #-16]!
	stp     x2,x3, [sp, #-16]!
	bl      PreTaskHook
	ldp     x2,x3, [sp], #16
	ldp     x0,x1, [sp], #16
	str     w3,[x1]  /* restore CallLevel */
	#endif
	ldr     x1, [x0, #0x0 ]
	mov     sp, x1
	ldr     x1, [x0,#0x08]
	br      x1

	.global Os_PortResume
	.type   Os_PortResume, %function
Os_PortResume:
	ldp x0, x1, [sp, #-16]!
	msr spsr_el1, x0
	msr elr_el1, x1

	ldr x30, [sp, #-8]!
	ldp x29, x28, [sp, #-16]!
	ldp x27, x26, [sp, #-16]!
	ldp x25, x24, [sp, #-16]!
	ldp x23, x22, [sp, #-16]!
	ldp x21, x20, [sp, #-16]!
	ldp x19, x18, [sp, #-16]!
	ldp x17, x16, [sp, #-16]!
	ldp x15, x14, [sp, #-16]!
	ldp x13, x12, [sp, #-16]!
	ldp x11, x10, [sp, #-16]!
	ldp x9, x8, [sp, #-16]!
	ldp x7, x6, [sp, #-16]!
	ldp x5, x4, [sp, #-16]!
	ldp x3, x2, [sp, #-16]!
	ldp x1, x0, [sp, #-16]!
	add sp, sp, 33*8
	ret

	.global Os_PortDispatch
	.type   Os_PortDispatch, %function
Os_PortDispatch:
	sub sp, sp, 33*8
	stp x0, x1, [sp], +16
	stp x2, x3, [sp], +16
	stp x4, x5, [sp], +16
	stp x6, x7, [sp], +16
	stp x8, x9, [sp], +16
	stp x10, x11, [sp], +16
	stp x12, x13, [sp], +16
	stp x14, x15, [sp], +16
	stp x16, x17, [sp], +16
	stp x18, x19, [sp], +16
	stp x20, x21, [sp], +16
	stp x22, x23, [sp], +16
	stp x24, x25, [sp], +16
	stp x26, x27, [sp], +16
	stp x28, x29, [sp], +16
	str x30, [sp], +8

	mrs x0, spsr_el1
	mrs x1, elr_el1
	stp x0, x1, [sp], +16

	ldr     x3, =RunningVar
	ldr     x4, [x3]

	mov     x5, sp
	str     x5, [x4, #0x0 ]

	ldr     x12, =Os_PortResume
	str     x12, [x4, #0x08]

	/* loading system stack */
	ldr x30, =stack_top
	mov sp, x30

	#ifdef OS_USE_POSTTASK_HOOK
	ldr     x1, = CallLevel
	ldr     w3, [x1]
	mov     w2, #8   /* CallLevel = TCL_PREPOST */
	str     w2,[x1]
	stp     x0,x1, [sp, #-16]!
	stp     x2,x3, [sp, #-16]!
	//bl      PostTaskHook
	ldp     x2,x3, [sp], #16
	ldp     x0,x1, [sp], #16
	str     w3,[x1]  /* restore CallLevel */
	#endif

	b       Os_PortStartDispatch

	.global vector_irq
	.type   vector_irq, %function
vector_irq:
	/* Save working register to IRQ stack */

EnterISR:
	/* save context on fisrt ISR enter                                          */


l_nosave:

ExitISR:


	#ifdef OS_USE_PRETASK_HOOK

	#endif


l_nopreempt:


l_nodispatch:

