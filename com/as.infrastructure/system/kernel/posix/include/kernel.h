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
#ifndef KERNEL_H_
#define KERNEL_H_
/* ============================ [ INCLUDES  ] ====================================================== */

/* ============================ [ MACROS    ] ====================================================== */
/*
 *  Macro for declare Task/Alarm/ISR Entry
 */
#define TASKNAME(TaskName)	TaskMain##TaskName
#define TASK(TaskName)		void TaskMain##TaskName(void)
#define ISRNAME(ISRName)	ISRMain##ISRName
#define ISR(ISRName)		void ISRMain##ISRName(void)
#define ALARMCALLBACKNAME(AlarmCallBackName) 	\
							AlarmMain##AlarmCallBackName
#define ALARMCALLBACK(AlarmCallBackName)	\
							void AlarmMain##AlarmCallBackName(void)

/*
 *  OS Status (error code)
 */
#define E_OS_ACCESS		((StatusType) 1)
#define E_OS_CALLEVEL	((StatusType) 2)
#define E_OS_ID			((StatusType) 3)
#define E_OS_LIMIT		((StatusType) 4)
#define E_OS_NOFUNC		((StatusType) 5)
#define E_OS_RESOURCE	((StatusType) 6)
#define E_OS_STATE		((StatusType) 7)
#define E_OS_VALUE		((StatusType) 8)

#define RES_SCHEDULER		((ResourceType) 0)	/* Constant of data type ResourceType (see (osek)chapter 8, Resource management). */


#define OSDEFAULTAPPMODE	((AppModeType) 0x01)
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32_t		StatusType;

typedef uint32_t		TaskType;			/* Task ID */
typedef TaskType*		TaskRefType;
typedef uint32_t		TaskStateType;
typedef TaskStateType*	TaskStateRefType;
typedef uint32_t		TaskStateType;		/* Task State */
typedef uint32_t		ResourceType;		/* Resource ID */
typedef uint32_t		EventMaskType;		/* Event Mask in bit */
typedef EventMaskType*	EventMaskRefType;
typedef uint32_t		TickType;			/* Time Tick */
typedef TickType*		TickRefType;
typedef uint32_t		AlarmType;			/* Alarm ID */
typedef uint32_t		AppModeType;		/* OS Application Mode */
typedef uint32_t		OSServiceIdType;	/* OS service API ID */
typedef uint32_t		IsrType;			/* ISR ID */
typedef uint32_t		CounterType;		/* Counter ID */
typedef struct
{
	TickType maxallowedvalue;
	TickType ticksperbase;
	TickType mincycle;
} AlarmBaseType;
typedef AlarmBaseType*	AlarmBaseRefType;

typedef	unsigned char		UINT8;		/* 8 bit unsigned integer */
typedef	unsigned short  	UINT16;		/* 16 bit unsigned integer */

typedef void		*VP;			/* void pointer */
typedef void		(*FP)(void);	/* function pointer */
typedef uint32_t	Priority;		/* priority for task/resource/ISR */
typedef	uint32_t	IPL;			/* interrupt processing level */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void EnableAllInterrupts(void);
void DisableAllInterrupts(void);
#endif /* KERNEL_H_ */
