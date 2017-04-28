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
#include "ucos_ii.h"
#include "Os.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_OS 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
TickType				OsTickCounter;
/* ============================ [ LOCALS    ] ====================================================== */
static OS_CPU_SR  cpu_sr = 0u;
static uint32_t isrDisableCounter = 0;


#if OS_MAX_FLAGS < TASK_NUM
#error please set OS_MAX_FLAGS bigger than TASK_NUM
#endif
static OS_FLAG_GRP* taskEvent[TASK_NUM];

static void uCOS_TaskProcess(void *p_arg)
{
	ASLOG(OS,"Starting Task<%d>\n",(uint32)p_arg);
	for( ;; )
	{
		WaitEvent(OS_EVENT_TASK_ACTIVATION);
		ClearEvent(OS_EVENT_TASK_ACTIVATION);
		TaskList[(uint32)p_arg].main();
	}

}
/* ============================ [ FUNCTIONS ] ====================================================== */

FUNC(StatusType,MEM_ACTIVATE_TASK) 	 ActivateTask    ( TaskType TaskId)
{
	StatusType ercd = E_OK;
	if (TaskId < TASK_NUM)
	{
#ifdef __WINDOWS__
		ercd = SetEvent2(TaskId,OS_EVENT_TASK_ACTIVATION);
#else
		ercd = SetEvent(TaskId,OS_EVENT_TASK_ACTIVATION);
#endif
	}
	else
	{
		ercd = E_OS_ID;
		asAssert(0);
	}
	return ercd;
}

FUNC(StatusType,MEM_ACTIVATE_TASK) 	 TerminateTask   ( void )
{
	StatusType ercd = E_OK;

	return ercd;
}

FUNC(StatusType,MEM_GetTaskID) 		GetTaskID     ( TaskRefType TaskID )
{
	StatusType ercd = E_OK;

	if(OSTCBCur->OSTCBPrio < TASK_NUM)
	{
		*TaskID = TASK_NUM-1-OSTCBCur->OSTCBPrio;
	}
	else
	{
		ercd = E_OS_ACCESS;
		asAssert(0);
	}

	return ercd;
}
FUNC(StatusType,MEM_GetResource) GetResource ( ResourceType ResID )
{
	StatusType ercd = E_OK;

	return ercd;
}

FUNC(StatusType,MEM_ReleaseResource) ReleaseResource ( ResourceType ResID )
{
	StatusType ercd = E_OK;

	return ercd;
}

#ifdef __WINDOWS__
FUNC(StatusType,MEM_SetEvent)        SetEvent2        ( TaskType tskid , EventMaskType mask )
#else
FUNC(StatusType,MEM_SetEvent)        SetEvent        ( TaskType tskid , EventMaskType mask )
#endif
{
	StatusType ercd = E_OK;

	if (tskid < TASK_NUM)
	{
		OSFlagPost(taskEvent[tskid],(OS_FLAGS)mask,OS_FLAG_SET,&ercd);
		if(ercd != OS_ERR_NONE)
		{
			ercd = E_OS_ACCESS;
			asAssert(0);
		}
	}
	else
	{
		ercd = E_OS_ID;
		asAssert(0);
	}
	return ercd;
}


StatusType ClearEvent ( EventMaskType mask )
{
	StatusType ercd = E_OK;

	TaskType tskid;

	GetTaskID(&tskid);

	if (tskid < TASK_NUM)
	{
		OSFlagPost(taskEvent[tskid],(OS_FLAGS)mask,OS_FLAG_CLR,&ercd);
		if(ercd != OS_ERR_NONE)
		{
			ercd = E_OS_ACCESS;
			asAssert(0);
		}
	}
	else
	{
		ercd = E_OS_ID;
		asAssert(0);
	}

	return ercd;
}

StatusType GetEvent ( TaskType tskid , EventMaskRefType p_mask )
{
	StatusType ercd = E_OK;

	if (tskid < TASK_NUM)
	{
		*p_mask = OSFlagQuery(taskEvent[tskid],&ercd);
		if(ercd != OS_ERR_NONE)
		{
			ercd = E_OS_ACCESS;
			asAssert(0);
		}
	}
	else
	{
		ercd = E_OS_ID;
		asAssert(0);
	}
	return ercd;
}

StatusType WaitEvent ( EventMaskType mask )
{
	StatusType ercd = E_OK;
	TaskType tskid;

	GetTaskID(&tskid);
	ASLOG(OS,"WaitEvent Task<%d> Mask<%X>\n",tskid,mask);
	if (tskid < TASK_NUM)
	{
		OSFlagPend(taskEvent[tskid],(OS_FLAGS)mask,OS_FLAG_WAIT_SET_ANY,0,&ercd);

		if(ercd != OS_ERR_NONE)
		{
			ercd = E_OS_ACCESS;
			asAssert(0);
		}
	}
	else
	{
		ercd = E_OS_ID;
		asAssert(0);
	}
	return ercd;
}

FUNC(void,MEM_OsTick) OsTick ( void )
{
	OsTickCounter ++;

	if(0 == OsTickCounter)
	{
		OsTickCounter = 1;
	}
}

FUNC(StatusType,MEM_GetAlarmBase) GetAlarmBase ( AlarmType AlarmId, AlarmBaseRefType Info )
{

	Info[0].maxallowedvalue = TICK_MAX;
	Info[0].ticksperbase     = 1u;
	Info[0].mincycle        = 1u;

	return E_OK;
}

FUNC(StatusType,MEM_GetAlarm) GetAlarm(AlarmType AlarmId, TickRefType Tick)
{
	return E_OK;
}

FUNC(StatusType,MEM_SetRelAlarm) SetRelAlarm ( AlarmType AlarmId, TickType Increment, TickType Cycle )
{
	return E_OK;
}

FUNC(StatusType,MEM_SetAbsAlarm) SetAbsAlarm ( AlarmType AlarmId, TickType Start, TickType Cycle )
{

	return E_OK;
}

FUNC(StatusType,MEM_CancelAlarm) CancelAlarm ( AlarmType AlarmId )
{

	return E_OK;
}

FUNC(TickType,MEM_GetOsTick) GetOsTick( void )
{
	return OsTickCounter;
}
FUNC(TickType,MEM_GetOsElapsedTick)  GetOsElapsedTick  ( TickType prevTick )
{
    if (OsTickCounter >= prevTick) {
		return(OsTickCounter - prevTick);
	}
	else {
		return(prevTick - OsTickCounter + (TICK_MAX + 1));
	}
}
FUNC(void,MEM_StartOS)              StartOS       ( AppModeType Mode )
{
	uint32 i;
	uint8  ercd;
	const task_declare_t* td;

	OSInit();

	for(i=0;i<TASK_NUM;i++)
	{
		td = &TaskList[i];
		ercd = OSTaskCreate(uCOS_TaskProcess, (void *)i, (OS_STK*)&td->pstk[td->stk_size-1], TASK_NUM-1-td->priority);
		asAssert(OS_ERR_NONE == ercd);
		taskEvent[i] = OSFlagCreate(0,&ercd);
		asAssert(OS_ERR_NONE == ercd);

		if(td->app_mode&Mode)
		{
			ActivateTask(i);
		}
	}

	StartupHook();

	OSStart();
}

FUNC(void,MEM_ShutdownOS)  ShutdownOS ( StatusType ercd )
{
	(void)ercd;
	while(1);
}


imask_t __Irq_Save(void)
{
	isrDisableCounter ++ ;
	if(1u == isrDisableCounter)
	{
		OS_ENTER_CRITICAL();
	}
	return 0;
}

void Irq_Restore(imask_t irq_state)
{

	isrDisableCounter --;
	if(0u == isrDisableCounter)
	{
		OS_EXIT_CRITICAL();
	}

	(void)irq_state;
}
