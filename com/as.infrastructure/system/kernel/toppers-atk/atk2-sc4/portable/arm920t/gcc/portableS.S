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


#define NOINT           0xc0

    .extern p_runtsk, p_schedtsk, knl_taskindp
    .extern callevel_stat,,activate_r,knl_dispatch_started
    .extern knl_system_stack_top

    .section .text
    .global Irq_Restore
    .type   Irq_Restore, %function
/* void Irq_Restore( imask_t intsts ); */
Irq_Restore:
    msr cpsr, r0
    mov pc, lr

    .global __Irq_Save
    .type   __Irq_Save, %function
/* imask_t __Irq_Save( void ); */
__Irq_Save:
    mrs r0, cpsr
    orr r1, r0, #NOINT
    msr cpsr_c, r1
    mov pc, lr

    .global knl_activate_r
    .type   knl_activate_r, %function
knl_activate_r:


    .global  knl_dispatch_r
    .type   knl_dispatch_r, %function
knl_dispatch_r:


dispatch_task:


    .global knl_start_dispatch
    .type   knl_start_dispatch, %function
knl_dispatch_ret_int:
knl_start_dispatch:


    .global knl_dispatch_entry
    .type   knl_dispatch_entry, %function
knl_dispatch_entry:


    .global EnterISR
    .type   EnterISR, %function
EnterISR:


l_nosave:



    .global ExitISR
    .type   ExitISR, %function
ExitISR:


l_nodispatch:


    .extern knl_system_tick_handler
    .global knl_system_tick
    .type   knl_system_tick, %function
knl_system_tick:


    .global knl_isr_process
    .extern knl_isr_handler
    .type   knl_isr_process, %function
knl_isr_process:



