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
#ifndef OS_I_H
#define OS_I_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
/* ============================ [ MACROS    ] ====================================================== */
#define DeclareTask(Name,Autostart,AppMode)		\
    {											\
        .main = TaskMain##Name,							\
        .priority = TASK_ID_##Name,				\
        .autostart = Autostart,					\
        .app_mode = AppMode						\
    }

#define DeclareAlarm(Name)						\
    {											\
        .main = AlarmMain##Name						\
    }

#define    E_OS_ACCESS             	  (StatusType)1
#define    E_OS_CALLEVEL              (StatusType)2
#define    E_OS_ID                    (StatusType)3
#define    E_OS_LIMIT                 (StatusType)4
#define    E_OS_NOFUNC                (StatusType)5
#define    E_OS_RESOURCE              (StatusType)6
#define    E_OS_STATE                 (StatusType)7
#define    E_OS_VALUE                 (StatusType)8

#define OSDEFAULTAPPMODE        (AppModeType)1

#define SUSPENDED                  ((StatusType) 0)
#define RUNNING                    ((StatusType) 1)
#define READY                      ((StatusType) 2)
#define WAITING                    ((StatusType) 3)

#define INVALID_TASK            (TaskType)-1

/*
 *  Macro for declare Task/Alarm/ISR Entry
 */
#define TASK(TaskName)        	 void TaskMain##TaskName(void)
#define ISR(ISRName)        	 void ISRMain##ISRName(void)
#define ALARM(AlarmCallBackName) void AlarmMain##AlarmCallBackName(void)

#define RES_SCHEDULER           (ResourceType)0 /* default resources for OS */

/* ============================ [ TYPES     ] ====================================================== */
typedef uint8 					StatusType;
typedef uint32   				EventMaskType;
typedef EventMaskType *			EventMaskRefType;
typedef uint8  			    	TaskType;
typedef TaskType *				TaskRefType;
typedef uint8					TaskStateType;
typedef TaskStateType *			TaskStateRefType;
typedef uint32                  AppModeType;	/*! each bit is a mode */

typedef uint32					TickType;
typedef TickType*				TickRefType;
typedef uint8			        IsrType;			/* ISR ID */
typedef uint8			        CounterType;		/* Counter ID */

typedef uint8					AlarmType;
typedef struct
{
	TickType maxallowedvalue;
	TickType ticksperbase;
	TickType mincycle;
} 								AlarmBaseType;
typedef AlarmBaseType *			AlarmBaseRefType;

typedef uint8                   ResourceType;

/*! extended OS types */
typedef void         (*task_main_t)(void);
typedef void         (*alarm_main_t)(void);
typedef uint8 		   task_priority_t;

typedef struct
{
    task_main_t    	main;
    task_priority_t priority;	/*! priority also represent the task id, the same as TaskType */
    boolean         autostart;
    AppModeType     app_mode;	/*! means task runnable modes */
}task_declare_t;

typedef struct
{
    alarm_main_t main;
    /* No Autostart support */
}alarm_declare_t;
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
FUNC(void,MEM_BITOP_SET)      BitopSet   ( uint8* pBuffer, uint32 Position );
FUNC(void,MEM_BITOP_CLEAR)    BitopClear ( uint8* pBuffer, uint32 Position );
FUNC(bool,MEM_BITOP_ISBITSET) IsBitopSet ( uint8* pBuffer, uint32 Position );

FUNC(void,MEM_OS_TICK) 				 OsTick 		  ( void );
FUNC(StatusType,MEM_GetAlarmBase) 	 GetAlarmBase    ( AlarmType AlarmId, AlarmBaseRefType Info );
FUNC(StatusType,MEM_GetAlarm) 		 GetAlarm	     ( AlarmType AlarmId, TickRefType Tick );
FUNC(StatusType,MEM_SetRelAlarm) 	 SetRelAlarm     ( AlarmType AlarmId, TickType Increment, TickType Cycle );
FUNC(StatusType,MEM_SetAbsAlarm) 	 SetAbsAlarm     ( AlarmType AlarmId, TickType Increment, TickType Cycle );
FUNC(StatusType,MEM_CancelAlarm)     CancelAlarm     ( AlarmType AlarmId );
FUNC(TickType,MEM_GetOsTick)         GetOsTick       ( void );
FUNC(TickType,MEM_GetOsElapsedTick)  GetOsElapsedTick( TickType prevTick );
FUNC(StatusType,MEM_ACTIVATE_TASK) 	 ActivateTask    ( TaskType TaskId);
FUNC(StatusType,MEM_ACTIVATE_TASK) 	 TerminateTask   ( void );
FUNC(StatusType,MEM_Schedule) 		 Schedule        ( void );
FUNC(StatusType,MEM_GetTaskID) 		 GetTaskID       ( TaskRefType TaskID );
FUNC(StatusType,MEM_GetTaskState) 	 GetTaskState    ( TaskType TaskID,TaskStateRefType State );
FUNC(void,MEM_StartOS)               StartOS         ( AppModeType Mode );
FUNC(StatusType,MEM_GetResource)     GetResource     ( ResourceType ResID );
FUNC(StatusType,MEM_ReleaseResource) ReleaseResource ( ResourceType ResID );
FUNC(StatusType,MEM_SetEvent)        SetEvent        ( TaskType tskid , EventMaskType mask );
FUNC(StatusType,MEM_ClearEvent)      ClearEvent      ( EventMaskType mask );
FUNC(StatusType,MEM_GetEvent)        GetEvent        ( TaskType tskid , EventMaskRefType p_mask );
FUNC(StatusType,MEM_WaitEvent)       WaitEvent       ( EventMaskType mask );

#endif /* OS_I_H */
