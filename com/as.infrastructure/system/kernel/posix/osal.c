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
#include "asdebug.h"
#include <pthread.h>
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_TRACE_OS 0
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	pthread_t self;
	boolean is_active;
	uint8_t activation;
}TCB_Type;
/* ============================ [ DECLARES  ] ====================================================== */
extern void object_initialize(void);
extern void StartupHook(void);
extern const AppModeType  tinib_autoact[TASK_NUM];
extern const UINT8	tnum_task;		/* number of the (basic & extend) task has been configured */
extern const UINT8	tnum_exttask;		/* number of the extend task has been configured */
extern const Priority	tinib_inipri[];		/* task init priority */
extern const Priority	tinib_exepri[];		/* task priority when start to execute */
extern const UINT8		tinib_maxact[];		/* task max activate count */
extern const AppModeType tinib_autoact[];	/* task autostart mode */
extern const FP			tinib_task[];		/* task entery */
extern const VP			tinib_stk[];		/* task stack buffer entry */
extern const UINT16		tinib_stksz[];		/* task stack buffer size */

extern const FP             alminib_cback[ALARM_NUM];
/* ============================ [ DATAS     ] ====================================================== */
TickType				OsTickCounter = 1;	/* zero mask as not started */

STATIC TickType			 	AlarmTick[ALARM_NUM];
STATIC TickType			 	AlarmPeriod[ALARM_NUM];
/* ============================ [ LOCALS    ] ====================================================== */
static pthread_mutex_t isrMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t isrAccess = PTHREAD_MUTEX_INITIALIZER;
static imask_t isrEnabled = TRUE;
static TCB_Type tcb[TASK_NUM];
static AppModeType appmode;
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

FUNC(StatusType,MEM_ActivateTask)  ActivateTask ( TaskType TaskID )
{
	TCB_Type* ptcb;
	StatusType ercd = E_OK;
	imask_t imask;
	ASLOG(TRACE_OS, "activate %d\n",TaskID);
	if(TaskID < TASK_NUM)
	{
		Irq_Save(imask);

		ptcb = &tcb[TaskID];
		if(ptcb->is_active)
		{
			if(ptcb->activation < tinib_maxact)
			{
				ptcb->activation ++;
			}
			else
			{
				ercd = E_OS_LIMIT;
			}
		}
		else
		{
			ptcb->is_active = TRUE;
			pthread_create(&ptcb->self,NULL,tinib_task[TaskID],NULL);
		}

		Irq_Restore(imask);
	}
	else
	{
		ercd = E_OS_ID;
	}
	return ercd;
}
FUNC(StatusType,MEM_TerminateTask) TerminateTask ( void )
{
	StatusType ercd;
	TaskType TaskID;
	imask_t imask;
	TCB_Type* ptcb;
	ercd = GetTaskID(&TaskID);
	ASLOG(TRACE_OS, "terminate %d\n",TaskID);
	if (E_OK == ercd)
	{
		Irq_Save(imask);

		ptcb = &tcb[TaskID];

		ptcb->is_active = FALSE;

		if(ptcb->activation > 0)
		{
			ptcb->activation -- ;
			ActivateTask(TaskID);
		}

		Irq_Restore(imask);

		pthread_exit(NULL);
	}
	else
	{
		ercd = E_OS_ACCESS;
	}

	return ercd;
}
FUNC(StatusType,MEM_Schedule) 		Schedule      ( void ) {return E_OK;}
FUNC(StatusType,MEM_GetTaskID) 		GetTaskID     ( TaskRefType TaskID ) {
	uint32 i;
	for(i=0;i<TASK_NUM;i++)
	{
		if(pthread_self() == tcb[i].self)
		{
			*TaskID = i;
			return E_OK;
		}
	}
	return E_NOT_OK;
}
FUNC(StatusType,MEM_GetTaskState) 	GetTaskState  ( TaskType TaskID,TaskStateRefType State )
{
	StatusType ercd = E_OK;
	TCB_Type* ptcb;
	if(TaskID < TASK_NUM)
	{
		ptcb = &tcb[TaskID];
		if(ptcb->is_active)
		{
			*State = RUNNING;
		}
		else
		{
			*State = SUSPENDED;
		}
	}
	else
	{
		ercd = E_OS_ID;
	}
	return ercd;
}
FUNC(void,MEM_StartOS)              StartOS       ( AppModeType Mode )
{
	appmode = Mode;
	ASTRACE(OS);

	StartupHook();

	object_initialize();

	while(1)
	{
		OsTick();
		usleep(1000);
	}
}
FUNC(void,MEM_ShutdownOS)  ShutdownOS ( StatusType ercd ) {return;}


StatusType SetEvent ( TaskType tskid , EventMaskType mask ) {return E_OK;}
StatusType ClearEvent ( EventMaskType mask )  {return E_OK;}
StatusType GetEvent ( TaskType tskid , EventMaskRefType p_mask ) {return E_OK;}
StatusType WaitEvent ( EventMaskType mask )  {return E_OK;}

FUNC(StatusType,MEM_GetResource) GetResource ( ResourceType ResID ) {return E_OK;}
FUNC(StatusType,MEM_ReleaseResource) ReleaseResource ( ResourceType ResID ) {return E_OK;}


FUNC(void,MEM_OsTick) OsTick ( void )
{
	AlarmType AlarmId;
	OsTickCounter ++;

	if(0 == OsTickCounter)
	{
		OsTickCounter = 1;
	}

	for(AlarmId=0; AlarmId<ALARM_NUM; AlarmId++)
	{
		if(AlarmTick[AlarmId] > 0)
		{
			AlarmTick[AlarmId]--;
			if(0u == AlarmTick[AlarmId])
			{
				alminib_cback[AlarmId]();
				AlarmTick[AlarmId] = AlarmPeriod[AlarmId];
			}
		}
	}
}

FUNC(StatusType,MEM_GetAlarmBase) GetAlarmBase ( AlarmType AlarmId, AlarmBaseRefType Info )
{
	asAssert(AlarmId<ALARM_NUM);

    Info[0].maxallowedvalue = TICK_MAX;
    Info[0].ticksperbase     = 1u;
    Info[0].mincycle        = 1u;

	return E_OK;
}

FUNC(StatusType,MEM_GetAlarm) GetAlarm(AlarmType AlarmId, TickRefType Tick)
{
	asAssert(AlarmId<ALARM_NUM);

	Tick[0] = AlarmTick[AlarmId];

	return E_OK;
}

FUNC(StatusType,MEM_SetRelAlarm) SetRelAlarm ( AlarmType AlarmId, TickType Increment, TickType Cycle )
{
	asAssert(AlarmId<ALARM_NUM);

	AlarmTick[AlarmId] = Increment;
	AlarmPeriod[AlarmId] = Cycle;

	return E_OK;
}

FUNC(StatusType,MEM_SetAbsAlarm) SetAbsAlarm ( AlarmType AlarmId, TickType Start, TickType Cycle )
{
	asAssert(AlarmId<ALARM_NUM);

	if (OsTickCounter < Start)
	{
		Start = Start - OsTickCounter;
	}
	else
	{
		Start = TICK_MAX - OsTickCounter + Start + 1;
	}

	AlarmTick[AlarmId] = Start;
	AlarmPeriod[AlarmId] = Cycle;

	return E_OK;
}

FUNC(StatusType,MEM_CancelAlarm) CancelAlarm ( AlarmType AlarmId )
{
	asAssert(AlarmId<ALARM_NUM);

	AlarmTick[AlarmId] = 0;
	AlarmPeriod[AlarmId] = 0;

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

FUNC(void,MEM_OsAlarmInit) OsAlarmInit ( void )
{
    memset(AlarmTick,0,sizeof(AlarmTick));
    memset(AlarmPeriod,0,sizeof(AlarmPeriod));
}


void task_initialize(void)
{
	TaskType	tskid;
	memset(tcb,0,sizeof(tcb));
	for (tskid = 0; tskid < tnum_task; tskid++)
	{
		if(tinib_autoact[tskid]&appmode)
		{
			ActivateTask(tskid);
		}
	}
}
void alarm_initialize(void)
{
	OsAlarmInit();
}
void resource_initialize(void)
{

}

TASK(TaskIdle)
{
	ASLOG(STDOUT,"TaskIdle is running\n");
	for(;;)
	{
		KSM_EXECUTE();
		usleep(1000);
	}

}
