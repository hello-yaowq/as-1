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
#ifndef OS_I_H
#define OS_I_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "contiki.h"
/* ============================ [ MACROS    ] ====================================================== */
#define    E_OS_ACCESS                (StatusType)1
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
#define TASK(TaskName)           PROCESS_THREAD(TaskName, ev, data)
#define ISR(ISRName)             void ISRMain##ISRName(void)
#define ALARM(AlarmCallBackName) void AlarmMain##AlarmCallBackName(void)

#define DeclareAlarm(Name)						\
    {											\
        .main = AlarmMain##Name						\
    }

#define RES_SCHEDULER           (ResourceType)0 /* default resources for OS */

#define GetResource(...) 0
#define ReleaseResource(...) 0
#define SetEvent(...) 0
#define WaitEvent(...) 0
#define GetEvent(...) 0
#define ClearEvent(...) 0
#define TerminateTask(...) PROCESS_EXIT()
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

typedef void         (*alarm_main_t)(void);

typedef struct
{
    alarm_main_t main;
    /* No Autostart support */
}alarm_declare_t;
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void OsTick ( void );

FUNC(StatusType,MEM_SetRelAlarm) SetRelAlarm ( AlarmType AlarmId, TickType Increment, TickType Cycle );
FUNC(StatusType,MEM_SetAbsAlarm) SetAbsAlarm ( AlarmType AlarmId, TickType Start, TickType Cycle );
FUNC(StatusType,MEM_CancelAlarm) CancelAlarm ( AlarmType AlarmId );
FUNC(StatusType,MEM_ACTIVATE_TASK) 	 ActivateTask    ( TaskType TaskId);

extern void EnableAllInterrupts(void);
extern void DisableAllInterrupts(void);
#endif /* OS_I_H */
