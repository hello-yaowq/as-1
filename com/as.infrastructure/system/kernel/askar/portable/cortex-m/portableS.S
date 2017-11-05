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

	.global knl_activate_r
	.type   knl_activate_r, %function
knl_activate_r:
	mov r3, #0x01000000 /* TS_PSR_T */
	ldr r2, =Os_PortActivate
	push {r2,r3}
	subs  sp,sp,#24
	bx lr

	.global  knl_dispatch_r
	.type   knl_dispatch_r, %function
knl_dispatch_r:
	/* start to restore task's context */
	pop     {r4-r11}
	cpsie   i
	dsb
	isb
	bx      lr

	.global knl_start_dispatch
	.type   knl_start_dispatch, %function
knl_start_dispatch:
	ldr     r0, =ReadyVar
	ldr     r0, [r0]
	cmp     r0, #0
	beq     Os_PortIdle
Os_PortDispatchFormISR:
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

	ldr     r12, =knl_dispatch_r
	str     r12, [r4, #0x04]

	b       knl_start_dispatch
