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
    EXTERN knl_system_stack,callevel
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

    PUBLIC knl_exit_and_dispatch
knl_exit_and_dispatch:
    b knl_exit_and_dispatch
    PUBLIC knl_dispatch_entry

knl_dispatch_entry:
     b knl_dispatch_entry

knl_dispatch_ret_int:
    bx      lr

    PUBLIC EnterISR
EnterISR:    
    ldr     r1, =knl_taskindp              /* Enter task independent part */
    ldr     r2, [r1]
    add     r3, r2, #1  /* knl_taskindp++ */
    str     r3, [r1]
    push    {r1, r2}           /* Here I say wonderful Idea, do you know the reason? */
    cpsie   i     /* Enable Interrupr */
    bx      lr


    PUBLIC ExitISR
ExitISR:
    pop     {r1,r2}          /* here same as knl_taskindp--, do you know why? */
    str     r2, [r1]                             /* Leave task independent part */
    pop     {lr}
    cpsid   i

    ldr     r0, =knl_taskindp              /* Is it a nesting interrupt? */
    ldr     r0, [r0]
    cmp     r0, #0
    
    bne     l_nodispatch

    ldr     r0, =runtsk                /* Is dispatch required? */
    ldr     r1, =schedtsk
    ldr     r0, [r0]
    ldr     r1, [r1]
    cmp     r0, r1
    bne     knl_dispatch_ret_int                 /* To dispatch processing */

l_nodispatch:
    cpsie   i
    bx      lr

    EXTERN SignalCounter

    PUBLIC knl_system_tick
knl_system_tick:

    push {lr}
    bl EnterISR

    mov r0,#0
    bl SignalCounter
    bl TimeTick_Increment
    b  ExitISR

    END
