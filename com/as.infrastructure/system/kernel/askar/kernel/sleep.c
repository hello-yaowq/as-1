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
#include "pthread.h"
#if(OS_PTHREAD_NUM > 0)
#include <unistd.h>
/* ============================ [ MACROS    ] ====================================================== */
#ifndef USECONDS_PER_TICK
#define USECONDS_PER_TICK (10000000/OS_TICKS_PER_SECOND)
#endif
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static TAILQ_HEAD(sleep_list, TaskVar) OsSleepListHead;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Os_SleepInit(void)
{
	TAILQ_INIT(&OsSleepListHead);
}
void Os_Sleep(TickType tick)
{
	imask_t imask;

	Irq_Save(imask);
	if(NULL != RunningVar)
	{
		Os_SleepAdd(RunningVar,tick);
		Sched_GetReady();
		Os_PortDispatch();
	}
	Irq_Restore(imask);

}

int usleep (useconds_t __useconds)
{
	Os_Sleep((__useconds+USECONDS_PER_TICK-1)/USECONDS_PER_TICK);
	return __useconds;
}

unsigned int sleep (unsigned int __seconds)
{
	Os_Sleep((__seconds*1000000+USECONDS_PER_TICK-1)/USECONDS_PER_TICK);
	return 0;
}

void Os_SleepTick(void)
{
	TaskVarType *pTaskVar;
	TaskVarType *pNext;
	pTaskVar = TAILQ_FIRST(&OsSleepListHead);

	while(NULL != pTaskVar)
	{
		pNext = TAILQ_NEXT(pTaskVar, sentry);

		if(pTaskVar->sleep_tick > 0)
		{
			pTaskVar->sleep_tick --;
		}
		if(0u == pTaskVar->sleep_tick)
		{
			pTaskVar->state &= ~PTHREAD_STATE_SLEEPING;
			OS_TRACE_TASK_ACTIVATION(pTaskVar);
			Sched_PosixAddReady(pTaskVar-TaskVarArray);
			TAILQ_REMOVE(&OsSleepListHead, pTaskVar, sentry);
		}

		pTaskVar = pNext;
		if(pTaskVar->sleep_tick > 0)
		{
			break;
		}
	}
}

void Os_SleepAdd(TaskVarType* pTaskVar, TickType ticks)
{
	TaskVarType *pVar;
	TaskVarType *pPosVar = NULL;

	TAILQ_FOREACH(pVar, &OsSleepListHead, sentry)
	{
		if ((TickType)(pVar->sleep_tick) > ticks)
		{
			pPosVar = pVar;
			pVar->sleep_tick -= ticks;
			break;
		}
		else
		{
			ticks -= pVar->sleep_tick;
		}
	}

	pTaskVar->sleep_tick = ticks;
	pTaskVar->state |= PTHREAD_STATE_SLEEPING;

	if(NULL != pPosVar)
	{
		TAILQ_INSERT_BEFORE(pPosVar,pTaskVar,sentry);
	}
	else
	{
		TAILQ_INSERT_TAIL(&OsSleepListHead,pTaskVar,sentry);
	}
}

void Os_SleepRemove(TaskVarType* pTaskVar)
{
	TAILQ_REMOVE(&OsSleepListHead, pTaskVar, sentry);
}
#endif
