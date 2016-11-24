/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
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
#include "Os.h"

#include <pthread.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void object_initialize(void);
/* ============================ [ DATAS     ] ====================================================== */
TickType				OsTickCounter = 1;	/* zero mask as not started */
/* ============================ [ LOCALS    ] ====================================================== */
static pthread_mutex_t isrMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t isrAccess = PTHREAD_MUTEX_INITIALIZER;
static imask_t isrEnabled = TRUE;
/* ============================ [ FUNCTIONS ] ====================================================== */
void EnableAllInterrupts(void)
{
	pthread_mutex_lock(&isrAccess);

	isrEnabled = TRUE;
	pthread_mutex_unlock(&isrMutex);

	pthread_mutex_unlock(&isrAccess);
}
void DisableAllInterrupts(void)
{
	pthread_mutex_lock(&isrAccess);

	pthread_mutex_lock(&isrMutex);
	isrEnabled = FALSE;

	pthread_mutex_unlock(&isrAccess);
}

imask_t __Irq_Save(void)
{
	imask_t ret;

	pthread_mutex_lock(&isrAccess);

	ret = isrEnabled;

	if(TRUE == isrEnabled)
	{
		pthread_mutex_lock(&isrMutex);
	}

	isrEnabled = FALSE;

	pthread_mutex_unlock(&isrAccess);

	return ret;
}
void Irq_Restore(imask_t irq_state)
{
	pthread_mutex_lock(&isrAccess);

	isrEnabled = irq_state;

	if(TRUE == isrEnabled)
	{
		pthread_mutex_unlock(&isrMutex);
	}

	pthread_mutex_unlock(&isrAccess);
}

FUNC(StatusType,MEM_ActivateTask)  ActivateTask ( TaskType TaskID ) {return E_OK;}
FUNC(StatusType,MEM_TerminateTask) TerminateTask ( void ) {return E_OK;}
FUNC(StatusType,MEM_Schedule) 		Schedule      ( void ) {return E_OK;}
FUNC(StatusType,MEM_GetTaskID) 		GetTaskID     ( TaskRefType TaskID ) {return E_OK;}
FUNC(StatusType,MEM_GetTaskState) 	GetTaskState  ( TaskType TaskID,TaskStateRefType State ) {return E_OK;}
FUNC(void,MEM_StartOS)              StartOS       ( AppModeType Mode )
{
	object_initialize();
}
FUNC(void,MEM_ShutdownOS)  ShutdownOS ( StatusType ercd ) {return;}


StatusType SetEvent ( TaskType tskid , EventMaskType mask ) {return E_OK;}
StatusType ClearEvent ( EventMaskType mask )  {return E_OK;}
StatusType GetEvent ( TaskType tskid , EventMaskRefType p_mask ) {return E_OK;}
StatusType WaitEvent ( EventMaskType mask )  {return E_OK;}

FUNC(StatusType,MEM_GetResource) GetResource ( ResourceType ResID ) {return E_OK;}
FUNC(StatusType,MEM_ReleaseResource) ReleaseResource ( ResourceType ResID ) {return E_OK;}


FUNC(StatusType,MEM_GetAlarmBase) GetAlarmBase ( AlarmType AlarmId, AlarmBaseRefType Info ) {return E_OK;}
FUNC(StatusType,MEM_GetAlarm) GetAlarm(AlarmType AlarmId, TickRefType Tick) {return E_OK;}
FUNC(StatusType,MEM_SetRelAlarm) SetRelAlarm ( AlarmType AlarmId, TickType Increment, TickType Cycle ) {return E_OK;}
FUNC(StatusType,MEM_SetAbsAlarm) SetAbsAlarm ( AlarmType AlarmId, TickType Start, TickType Cycle ) {return E_OK;}
FUNC(StatusType,MEM_CancelAlarm) CancelAlarm ( AlarmType AlarmId ) {return E_OK;}


FUNC(TickType,MEM_GetOsTick) GetOsTick( void ) { return 0;}



void task_initialize(void)
{

}
void alarm_initialize(void)
{

}
void resource_initialize(void)
{

}
