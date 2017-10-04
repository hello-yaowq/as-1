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
#ifndef KERNEL_H_
#define KERNEL_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
/* ============================ [ MACROS    ] ====================================================== */
#define    E_OS_ACCESS                ((StatusType)1)
#define    E_OS_CALLEVEL              ((StatusType)2)
#define    E_OS_ID                    ((StatusType)3)
#define    E_OS_LIMIT                 ((StatusType)4)
#define    E_OS_NOFUNC                ((StatusType)5)
#define    E_OS_RESOURCE              ((StatusType)6)
#define    E_OS_STATE                 ((StatusType)7)
#define    E_OS_VALUE                 ((StatusType)8)

#define OSDEFAULTAPPMODE           ((AppModeType)1)

#define SUSPENDED                  ((StatusType) 0)
#define RUNNING                    ((StatusType) 1)
#define READY                      ((StatusType) 2)
#define WAITING                    ((StatusType) 3)

/*
 *  Macro for declare Task/Alarm/ISR Entry
 */
#define TASK(TaskName)           void TaskMain##TaskName(void)
#define ISR(ISRName)             void ISRMain##ISRName(void)
#define ALARM(AlarmCallBackName) void AlarmMain##AlarmCallBackName(void)

#define RES_SCHEDULER            (ResourceType)0 /* default resources for OS */

#define DeclareTask( name )       extern const TaskType name
#define DeclareAlarm( name )      extern const AlarmType name
#define DeclareEvent( name )      extern const EventType name
#define DeclareResource( name )   extern const ResourceType name

/* ============================ [ TYPES     ] ====================================================== */
typedef uint8					StatusType;
typedef uint32					EventMaskType;
typedef EventMaskType*			EventMaskRefType;
typedef uint8					TaskType;
typedef TaskType*				TaskRefType;
typedef uint8					TaskStateType;
typedef TaskStateType*			TaskStateRefType;
typedef uint32					AppModeType;	/*! each bit is a mode */

typedef uint32					TickType;
typedef TickType*				TickRefType;
typedef uint8					IsrType;			/* ISR ID */
typedef uint8					CounterType;		/* Counter ID */

typedef uint8					AlarmType;

typedef struct
{
	TickType maxAllowedValue;
	TickType ticksPerBase;
	TickType minCycle;
}								AlarmBaseType;
typedef AlarmBaseType*			AlarmBaseRefType;

typedef uint8					ResourceType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
StatusType ActivateTask ( TaskType TaskID );
StatusType TerminateTask( void );
StatusType ChainTask    ( TaskType TaskID );
StatusType Schedule     ( void );
StatusType GetTaskID    ( TaskRefType pTaskType );
StatusType GetTaskState ( TaskType TaskID,TaskStateRefType pState );

StatusType IncrementCounter(CounterType CounterID);
StatusType GetAlarmBase( AlarmType AlarmID, AlarmBaseRefType pInfo );
StatusType GetAlarm    ( AlarmType AlarmID ,TickRefType pTick );
StatusType SetRelAlarm ( AlarmType AlarmID , TickType Increment ,TickType Cycle );
StatusType SetAbsAlarm ( AlarmType AlarmID , TickType Start ,TickType Cycle );
StatusType CancelAlarm ( AlarmType AlarmID );

StatusType SetEvent  ( TaskType TaskID , EventMaskType pMask );
StatusType ClearEvent( EventMaskType Mask );
StatusType GetEvent  ( TaskType TaskID , EventMaskRefType pEvent );
StatusType WaitEvent ( EventMaskType Mask );

StatusType GetResource (ResourceType ResID);
StatusType ReleaseResource ( ResourceType ResID );

void StartOS ( AppModeType Mode );
void ShutdownOS( StatusType Error );

void ShutdownHook ( StatusType Error);
void StartupHook(void);
void ErrorHook(StatusType Error);
void PreTaskHook(void);
void PostTaskHook(void);

void EnterISR(void);
void ExitISR(void);

void DisableAllInterrupts( void );
void EnableAllInterrupts ( void );

void SuspendAllInterrupts( void );
void ResumeAllInterrupts ( void );

void SuspendOSInterrupts( void );
void ResumeOSInterrupts ( void );
#endif /* KERNEL_H_ */
