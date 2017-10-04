/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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
/* ============================ [ INCLUDES  ] ====================================================== */
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
	.extern Irq_Enable
	.extern Os_PortIsrHandler
/* ============================ [ DATAS     ] ====================================================== */
	.extern RunningVar
	.extern CallLevel
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
	.section .text

Os_PortIdle:
	bl      Irq_Enable
	ldr     r0, =RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	ble     Os_PortIdle
	mov     pc, lr

	.global Os_PortStartDispatch
	.type   Os_PortStartDispatch, %function
/* void Os_PortStartDispatch( void ); */
Os_PortStartDispatch:
	ldr     r0, =RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	ble     Os_PortIdle
	ldr     sp, [r0, #0x0 ]
	ldr     r1,[r0,#0x04]
	mov     pc,r1

	.global Os_PortResume
	.type   Os_PortResume, %function
Os_PortResume:
	ldmfd   sp!, {r0}
	msr     spsr, r0
	ldmfd   sp!, {r0-r12, lr, pc}^

	.global vector_irq
	.type   vector_irq, %function
vector_irq:
	/* Save working register to IRQ stack */
	stmfd   sp, {r0-r2}
EnterISR:
	ldr     r0, = RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     l_nosave    /* no task is running */

	ldr     r0, =CirticalCounter
	ldr     r2, [r0]
	add     r2, r2, #1  /* CirticalCounter++ */
	str     r2, [r0]

	cmp     r2, #1      /* previous CirticalCounter==0 */
	bne     l_nosave

	/* save context on fisrt ISR enter                                          */
	/* now R0 R1 R2 have all saved to IRQ's stack before,but no adjust to SP    */
	/*------------------------------------------------------------------------  */
	/*   R0--SP                                                                 */
	/*   R1--PC                                                                 */
	/*   R2--SPSR                                                               */
	/*------------------------------------------------------------------------  */

	sub     r0, sp, #12
	sub     r1, lr, #4              /* Adjust PC for return address to task */
	mrs     r2, spsr                /* Copy SPSR (Task CPSR) */
	;@ (PSR_SYS_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
	msr     cpsr_c, #0xDF           /* Change to SYS mode,that is task's mode */
									/* SAVE TASK''S CONTEXT ONTO OLD TASK''S STACK */
	stmfd   sp!, {r1}               /* Push task''s PC */
	stmfd   sp!, {r3-r12, lr}       /* Push task''s LR,R12-R4 */
	ldmfd   r0!, {r3-r5}            /* Load Task''s R0-R2 from IRQ stack */
	stmfd   sp!, {r3-r5}            /* Push Task''s R0-R2 to SVC stack */
	stmfd   sp!, {r2}               /* Push task''s SPSR */

	ldr     r3, =RunningVar
	ldr     r4, [r3]

	str     sp, [r4, #0x0 ]

	ldr     r12, =Os_PortResume
	str     r12, [r4, #0x04]

l_nosave:
	stmfd   sp!, {lr}     /* push {lr} */
	ldr     r1, = CallLevel
	ldr     r3, [r1]
	stmfd   sp!, {r3}
	mov     r3, #2   /* CallLevel = TCL_ISR2 */
	str     r3,[r1]

//	mrs r0, cpsr
//	bic r0, r0, #0xc0  /* interrupt enable */
//	msr cpsr_c, r0

	bl Os_PortIsrHandler

ExitISR:
	ldmfd   sp!, {r3}
	ldr     r1, = CallLevel
	str     r3, [r1]

	ldmfd   sp!, {lr}

//	mrs r0, cpsr
//	orr r0, r0, #0xc0  /* interrupt disable */
//	msr cpsr_c, r0

	ldr     r0, = RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     l_nodispatch

	ldr     r0, =CirticalCounter
	ldr     r1, [r0]
	sub     r1, r1, #1
	str     r1, [r0]
	cmp     r1, #0
	bne     l_nodispatch

	b       Os_PortStartDispatch                 /* To dispatch processing */

l_nodispatch:
	subs pc, lr, #4
