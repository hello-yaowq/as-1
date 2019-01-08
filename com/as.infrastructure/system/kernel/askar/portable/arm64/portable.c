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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "kernel_internal.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_OS 0
#define AS_LOG_OSE 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void Os_PortResume(void);
extern void Os_PortActivate(void);
extern void Os_PortStartSysTick(void);
/* ============================ [ DATAS     ] ====================================================== */
uint32 ISR2Counter;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Os_PortActivateImpl(void)
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
	Os_PortStartSysTick();
}

void Os_PortInitContext(TaskVarType* pTaskVar)
{
	/* 8 byte aligned */
	pTaskVar->context.sp = (void*)((uint64_t)(pTaskVar->pConst->pStack + pTaskVar->pConst->stackSize - 8)&(~(uint64_t)0x7UL));
	pTaskVar->context.pc = Os_PortActivate;
}


void Os_PortDispatch(void)
{
	__asm("svc 0");
}

void Os_PortStartDispatch(void)
{
	RunningVar = NULL;
	Os_PortDispatch();
	asAssert(0);
}

void Os_PortException(long exception, void* sp, long esr)
{
	ASLOG(OSE, "Exception %d happened!\n", exception);
	asAssert(0);
}

void EnterISR(void)
{
	/* do nothing */
}

void LeaveISR(void)
{
	/* do nothing */
}

#ifdef USE_PTHREAD_SIGNAL
void Os_PortCallSignal(int sig, void (*handler)(int), void* sp, void (*pc)(void))
{
	asAssert(NULL != handler);

	handler(sig);

	/* restore its previous stack */
	RunningVar->context.sp = sp;
	RunningVar->context.pc = pc;
}

void Os_PortExitSignalCall(void)
{
	Sched_GetReady();
	Os_PortStartDispatch();
}

int Os_PortInstallSignal(TaskVarType* pTaskVar, int sig, void* handler)
{
	void* sp;
	uint64_t* stk;

	sp = pTaskVar->context.sp;

	if((sp - pTaskVar->pConst->pStack) < (pTaskVar->pConst->stackSize*3/4))
	{
		/* stack 75% usage, ignore this signal call */
		ASLOG(OS,"install signal %d failed\n", sig);
		return -1;
	}

	stk = sp;

	*(--stk) = (uint64_t)handler;                /* x1 */
	*(--stk) = (uint64_t)sig;                    /* x0 */
	*(--stk) = (uint64_t)pTaskVar->context.pc;   /* x3 */
	*(--stk) = (uint64_t)sp;                     /* x2 */

	*(--stk) = 5 ;                               /* x5  */
	*(--stk) = 4 ;                               /* x4  */
	*(--stk) = 7 ;                               /* x7  */
	*(--stk) = 6 ;                               /* x6  */
	*(--stk) = 9 ;                               /* x9  */
	*(--stk) = 8 ;                               /* x8  */
	*(--stk) = 11;                               /* x11 */
	*(--stk) = 10;                               /* x10 */
	*(--stk) = 13;                               /* x13 */
	*(--stk) = 12;                               /* x12 */
	*(--stk) = 15;                               /* x15 */
	*(--stk) = 14;                               /* x14 */
	*(--stk) = 17;                               /* x17 */
	*(--stk) = 16;                               /* x16 */
	*(--stk) = 19;                               /* x19 */
	*(--stk) = 18;                               /* x18 */
	*(--stk) = 21;                               /* x21 */
	*(--stk) = 20;                               /* x20 */
	*(--stk) = 23;                               /* x23 */
	*(--stk) = 22;                               /* x22 */
	*(--stk) = 25;                               /* x25 */
	*(--stk) = 24;                               /* x24 */
	*(--stk) = 27;                               /* x27 */
	*(--stk) = 26;                               /* x26 */
	*(--stk) = 29;                               /* x29 */
	*(--stk) = 28;                               /* x28 */

	*(--stk) = (uint64_t)Os_PortExitSignalCall;  /* x30 */
	*(--stk) = (uint64_t)Os_PortCallSignal;      /* elr_el1 */
	*(--stk) = 0x20000305;                       /* spsr_el1 */

	pTaskVar->context.sp = stk;
	pTaskVar->context.pc = Os_PortResume;

	return 0;
}
#endif
