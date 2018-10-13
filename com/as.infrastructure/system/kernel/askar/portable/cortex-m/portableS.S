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
#define MACROS_ONLY
#include "Os_Cfg.h"
/* http://users.ece.utexas.edu/~valvano/EE345L/Labs/Fall2011/CortexM3_TRM_r2p1.pdf */
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
	.extern Irq_Enable
	.extern Irq_Disable
	.extern Os_PortIsrHandler
	.extern Sched_Preempt
	.extern Os_PortActivate
/* ============================ [ DATAS     ] ====================================================== */
	.extern RunningVar
	.extern ReadyVar
	.extern CallLevel
	.extern ISR2Counter
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
	.section .text

Os_PortIdle:
	/* set RunningVar to NULL */
	ldr     r1, =RunningVar
	str     r0, [r1]
	bl      Irq_Enable
l_idle:
	ldr     r0, =ReadyVar
	ldr     r0, [r0]
	cmp     r0, #0
	ble     l_idle
	bl      Irq_Disable
	bl      Sched_GetReady
	b       knl_start_dispatch

	.global knl_activate
	.type   knl_activate, %function
knl_activate:
	mov r3, #0x01000000 /* TS_PSR_T */
	ldr r2, =Os_PortActivate
	push {r2,r3}
	subs  sp,sp,#24
	# will going to task level
	ldr     r1, = CallLevel
	mov     r2, #1   /* CallLevel = TCL_TASK */
	str     r2,[r1]
	bx lr

	.global  knl_resume
	.type   knl_resume, %function
	.global  knl_resume_pre_hook
	.type   knl_resume_pre_hook, %function
knl_resume_pre_hook:
	#ifdef OS_USE_PRETASK_HOOK
	ldr     r1, = CallLevel
	ldr     r3, [r1]
	mov     r2, #8   /* CallLevel = TCL_PREPOST */
	str     r2,[r1]
	push    {r0-r3,lr}
	bl      PreTaskHook
	pop     {r0-r3,lr}
	str     r3,[r1]  /* restore CallLevel */
	#endif
knl_resume:
	/* start to restore task's context */
	pop     {r4-r11}
    cpsie   i
    dsb
    isb
	bx      lr

	.global knl_start_dispatch
	.type   knl_start_dispatch, %function
knl_start_dispatch:
	ldr lr, =0xFFFFFFF9 /* force it to process mode and use MSP */
	ldr     r0, =ReadyVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     Os_PortIdle
knl_start_dispatch_from_isr:
	ldr     r1, =RunningVar
	str     r0, [r1]
	ldr     sp, [r0, #0x0 ]
	ldr     r1,[r0,#0x04]
	bx      r1

	.global knl_dispatch_entry
	.type   knl_dispatch_entry, %function
knl_dispatch_entry:
	/* EXCEPTION stack:H->L: PSR PC LR R12 R3 R2 R1 R0 */
	push    {r4-r11}
	ldr     r3, =RunningVar
	ldr     r4, [r3]

	str     sp, [r4, #0x0 ]

	ldr     r12, =knl_resume
	str     r12, [r4, #0x04]

	b       knl_start_dispatch

	.type   EnterISR, %function
EnterISR:
	ldr     r0, = RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     l_nosave    /* no task is running */

	ldr     r1, =ISR2Counter
	ldr     r2, [r1]
	add     r2, r2, #1  /* ISR2Counter++ */
	str     r2, [r1]

	cmp     r2, #1      /* previous CirticalCounter==0 */
	bne     l_nosave

	/* save context on fisrt ISR enter */
	push    {r4-r11}

	str     sp, [r0, #0x0 ]

	ldr     r12, =knl_resume
	str     r12, [r0, #0x04]

    /* and then load isr system stack */
    ldr     sp, =knl_system_stack_top  /* Set system stack */

l_nosave:
	push    {r3} /* lr */
	ldr     r1, = CallLevel
	ldr     r3, [r1]
	push    {r3} /* previous CallLevel */
	mov     r3, #2   /* CallLevel = TCL_ISR2 */
	str     r3,[r1]
	cpsie   i
	bx      lr

	.type   EnterISR, %function
ExitISR:
	cpsid   i
	pop    {r3}
	ldr     r1, = CallLevel
	str     r3, [r1]

	pop     {lr}

	ldr     r0, = RunningVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     l_nodispatch

	ldr     r3, =ISR2Counter
	ldr     r1, [r3]
	sub     r1, r1, #1
	str     r1, [r3]
	cmp     r1, #0
	bne     l_nodispatch

	ldr     r1, = CallLevel
	ldr     r3, [r1]
	cmp     r3, #1  /* TCL_TASK */
	bne     l_nodispatch

	ldr     r1, = ReadyVar
	ldr     r1, [r1]

	cmp     r0, r1
	beq     l_nopreempt

	ldrb    r2, [r1, #8]  /* priority of ReadyVar */
	ldrb    r3, [r0, #8]  /* priority of RunningVar */
	cmp     r3, r2
	bge     l_nopreempt

	#ifdef OS_USE_PRETASK_HOOK
	ldr     r12, =knl_resume_pre_hook
	str     r12, [r0, #0x04]
	#endif

	bl      Sched_Preempt

	b       knl_start_dispatch

l_nopreempt:
	b       knl_start_dispatch_from_isr

l_nodispatch:
	bx      lr

	.type   knl_system_tick_handler, %function
	.weak knl_system_tick_handler
knl_system_tick_handler:
	bx lr

	.global knl_system_tick
	.type   knl_system_tick, %function
knl_system_tick:
	mov r3,lr
	bl EnterISR
	bl knl_system_tick_handler
	b  ExitISR

	.global knl_isr_process
	.extern knl_isr_handler
	.type   knl_isr_process, %function
knl_isr_process:
	mov r3,lr
	bl EnterISR
	mrs     r0, ipsr                             /* r0 = dintno */
	bl knl_isr_handler
	b  ExitISR
