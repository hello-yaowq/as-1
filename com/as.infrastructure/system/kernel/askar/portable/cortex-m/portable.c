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
#include "kernel_internal.h"
#include "asdebug.h"
#if  defined(CHIP_STM32F10X)
#include "stm32f10x.h"
#elif defined(CHIP_AT91SAM3S)
#include "SAM3S.h"
#include "board.h"
#elif defined(CHIP_LM3S6965)
#include "hw_ints.h"
#else
#error "CHIP is not known, please select the CHIP_STM32F10X or CHIP_AT91SAM3S..."
#endif
#include <core_cm3.h>
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_OS 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void knl_activate(void);
/* ============================ [ DATAS     ] ====================================================== */
uint32 ISR2Counter;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Os_PortActivate(void)
{
	/* get internal resource or NON schedule */
	RunningVar->priority = RunningVar->pConst->runPriority;

	ASLOG(OS, "%s(%d) is running\n", RunningVar->pConst->name,
			RunningVar->pConst->initPriority);

	OSPreTaskHook();

	CallLevel = TCL_TASK;
	Irq_Enable();

	RunningVar->pConst->entry();

	/* Should not return here */
	TerminateTask();
}

void Os_PortInit(void)
{
	ISR2Counter = 0;
}

void Os_PortInitContext(TaskVarType* pTaskVar)
{
	pTaskVar->context.sp = pTaskVar->pConst->pStack + pTaskVar->pConst->stackSize-4;
	pTaskVar->context.pc = knl_activate;
}

void EnterISR(void)
{
	/* do nothing */
}

void LeaveISR(void)
{
	/* do nothing */
}

void Os_PortDispatch(void)
{
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	__asm("cpsie   i");
	__asm("nop");
	__asm("cpsid   i");
}

void Os_PortStartDispatch(void)
{
	RunningVar = NULL;
	__asm("cpsie   i");
	__asm("svc 0");
	while(1); /* should not reach here */
}

void knl_isr_handler(int intno)
{

}

int Os_PortInstallSignal(TaskVarType* pTaskVar, int sig, void* handler)
{
	/* TODO: not implemented */
	asAssert(0);

	return 0;
}
