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
uint8 knl_system_stack[SYSTEM_STACK_SIZE];
uint8 knl_taskindp;

VP tcxb_sp[TASK_NUM];
FP tcxb_pc[TASK_NUM];
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
	WDT_Disable(WDT);
	/* Set 3 FWS for Embedded Flash Access */
	EFC->EEFC_FMR = EEFC_FMR_FWS(3);
	CLOCK_SetConfig(2);
	/* I don't know why, the baudrate is 38400 = 115200/3 */
	UART_Configure(115200, 64000000/3);// so I add this to solve the problem

	knl_taskindp = 0;

	if (SysTick_Config(64000000 / 1000))
	{
		/* Capture error */
		while (1);
	}
}
void sys_initialize(void)
{

}
void tool_initialize(void)
{

}
