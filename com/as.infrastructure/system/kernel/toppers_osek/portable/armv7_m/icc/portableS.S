/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
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
#define _MACRO_ONLY
#include "portable.h"

    EXTERN runtsk, schedtsk, knl_taskindp
    EXTERN callevel,tcxb_sp,tcxb_pc,activate_r,knl_dispatch_started
    SECTION .text:CODE
    THUMB
    PUBLIC enaint
//EXPORT void enaint( imask_t intsts );
enaint:
    mrs     r1, primask
    msr     primask, r0
    mov     r0, r1
    bx      lr

    SECTION .text:CODE
    THUMB
    PUBLIC disint
//EXPORT imask_t disint( void );
disint:
    mrs     r0, primask
    ldr     r1, =TS_PMK_D
    msr     primask, r1
    bx      lr
    
    SECTION .text:CODE
    THUMB
    PUBLIC knl_activate_r
knl_activate_r:
    mov r3, #TS_PSR_T
    ldr r2, =activate_r
    push {r2,r3}
    subs  SP,SP,#24
    bx lr

    PUBLIC  knl_dispatch_r
knl_dispatch_r:
     /* start to restore task's context */
    pop     {r4-r11}
    cpsie   i
    bx      lr

dispatch_task:
    ldr     r2, =tcxb_pc
    ldr     r3,[r2,r0,LSL #2]
    bx      r3

    PUBLIC knl_start_dispatch
knl_start_dispatch:
    ldr     r0, =schedtsk
    ldrb    r0, [r0]
    ldr     r1, =runtsk
    strb    r0, [r1]

    ldr     r2, =tcxb_sp
    ldr     sp, [r2,r0, LSL #2]
    b       dispatch_task

    PUBLIC knl_dispatch_entry
knl_dispatch_ret_int:
knl_dispatch_entry:
    push    {r4-r11}

    ldr     r1, =runtsk
    ldrb    r0, [r1]

    ldr     r2, =tcxb_sp
    str     sp, [r2,r0, LSL #2]

    ldr     r2, =tcxb_pc

    ldr     r12, =knl_dispatch_r
    str     r12, [r2,r0, LSL #2]

    b       knl_start_dispatch

    PUBLIC EnterISR
EnterISR:    
    ldr     r1, =knl_taskindp
    ldr     r2, [r1]
    add     r3, r2, #1
    str     r3, [r1]
    push    {r1, r2}
    ldr     r1, = callevel
    ldrb    r3, [r1]
    push    {r3}
    mov     r2, #2    /* callevel = TCL_ISR2 */
    strb    r2,[r1]
    cpsie   i
    bx      lr


    PUBLIC ExitISR
ExitISR:
    pop     {r3}
    ldr     r1, = callevel
    strb    r3, [r1]
    pop     {r1,r2}
    str     r2, [r1]
    pop     {lr}
    cpsid   i

    ldr     r0, =knl_taskindp
    ldr     r0, [r0]
    cmp     r0, #0
    
    bne     l_nodispatch

    ldr     r0, = knl_dispatch_started
    ldr     r0, [r0]
    cmp     r0, #0

    beq     l_nodispatch

    ldr     r0, =runtsk
    ldr     r1, =schedtsk
    ldrb    r0, [r0]
    ldrb    r1, [r1]
    cmp     r0, r1
    bne     knl_dispatch_ret_int                 /* To dispatch processing */

l_nodispatch:
    cpsie   i
    bx      lr

    EXTERN knl_system_tick_handler
    PUBLIC knl_system_tick
knl_system_tick:

    push {lr}
    bl EnterISR
    bl knl_system_tick_handler
    b  ExitISR

    END
