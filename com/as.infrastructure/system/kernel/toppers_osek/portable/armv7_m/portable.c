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
#include "osek_kernel.h"
#include "task.h"
#include "board.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void knl_activate_r(void);
/* ============================ [ DATAS     ] ====================================================== */
uint32 knl_taskindp;
uint32 knl_system_stack[SYSTEM_STACK_SIZE/sizeof(uint32)];
VP tcxb_sp[TASK_NUM];
FP tcxb_pc[TASK_NUM];
extern const FP tisr_pc[255];

extern const uint32 __vector_table[];

uint32 knl_dispatch_started;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void set_ipl(IPL ipl)
{
	if(ipl > 0)
	{
		disable_int();
	}
}
IPL  current_ipl(void)
{
	return 0;
}
void activate_r(void)
{
    tcb_curpri[runtsk] = tinib_exepri[runtsk];
    callevel = TCL_PREPOST;
    PreTaskHook();
    callevel = TCL_TASK;
    enable_int();
    tinib_task[runtsk]();
}
void activate_context(TaskType TaskID)
{
    tcxb_pc[TaskID] = (FP)knl_activate_r;

	tcxb_sp[TaskID] = (VP)( (UINT32)tinib_stk[TaskID] + (UINT32)tinib_stksz[TaskID]);
}
void cpu_terminate(void)
{

}
void sys_exit(void)
{

}

void cpu_initialize(void)
{
	const uint32_t* pSrc;
	pSrc = __vector_table ;
	SCB->VTOR = ( (uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk ) ;

	if ( ((uint32_t)pSrc >= IRAM_ADDR) && ((uint32_t)pSrc < IRAM_ADDR+IRAM_SIZE) )
	{
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos ;
	}

	WDT_Disable(WDT);

	/* Low level Initialize */
	LowLevelInit() ;

	knl_taskindp = 0;
	knl_dispatch_started = FALSE;
	if (SysTick_Config(64000000 / 1000))
	{
		/* Capture error */
		while (1);
	}

}

void knl_system_tick_handler(void)
{
	if(knl_dispatch_started == TRUE)
	{
		SignalCounter(0);
	}

	TimeTick_Increment();
}

void knl_isr_handler(uint32 intno)
{
	if( (intno>15) &&  (intno<51) && (tisr_pc[intno-16]!=NULL))
	{
		tisr_pc[intno-16]();
	}
	else
	{
		ShutdownOS(0xFF);
	}
}

void sys_initialize(void)
{

}
void tool_initialize(void)
{

}
