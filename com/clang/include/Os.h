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
#ifndef COM_CLANG_INCLUDE_OS_H_
#define COM_CLANG_INCLUDE_OS_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "Os_Cfg.h"
/* ============================ [ MACROS    ] ====================================================== */
#define OSDEFAULTAPPMODE (0x00000001u)

#define E_OS_ACCESS 	(StatusType)1
#define	E_OS_CALLEVEL 	(StatusType)2
#define	E_OS_ID			(StatusType)3
#define	E_OS_LIMIT 		(StatusType)4
#define	E_OS_NOFUNC 	(StatusType)5
#define	E_OS_RESOURCE 	(StatusType)6
#define	E_OS_STATE 		(StatusType)7
#define	E_OS_VALUE 		(StatusType)8

#define OsWaitEvent(TaskName,mask)									\
		xEventGroupWaitBits	(	os_task_events[TASK_ID_##TaskName],	\
								mask,								\
								pdFALSE,							\
								pdFALSE,							\
								portMAX_DELAY						\
							)

#define OsClearEvent(TaskName,mask) xEventGroupClearBits( os_task_events[TASK_ID_##TaskName], mask )
#define OsGetEvent(TaskName)		xEventGroupClearBits( os_task_events[TASK_ID_##TaskName], 0 )
#define OsSetEvent(TaskName,mask)	xEventGroupSetBits( os_task_events[TASK_ID_##TaskName], mask )
#define OsSetEventFromISR(TaskName,mask)								\
	do {																\
		BaseType_t xHigherPriorityTaskWoken, xResult;					\
		xHigherPriorityTaskWoken = pdFALSE;								\
		xEventGroupSetBitsFromISR( 	os_task_events[TASK_ID_##TaskName],	\
									mask, 								\
									&xHigherPriorityTaskWoken);			\
		if( xResult == pdPASS )											\
		{																\
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );				\
		}																\
	while(0)

/*
 * Task maximum activation is 1.
 */
#define OsActivateTask(TaskName) 	OsSetEvent(TaskName,EVENT_MASK_##TaskName##_Activation)
/*
 * This will only clear the activation bit of the task.
 * Task will terminate itself when it returns from its main function TASK()
 */
#define OsTerminateTask(TaskName)   OsClearEvent(TaskName,EVENT_MASK_##TaskName##_Activation)

#define OsSetRelAlarm(AlarmName,Increment,Cycle)													\
	do {																							\
			BaseType_t xResult;																		\
			TickType offset;																		\
			offset = Increment;																		\
			if(offset > 0)																			\
			{																						\
				offset = 1;																			\
			}																						\
			xResult = xTimerChangePeriod(os_alarm_handles[ALARM_ID_##AlarmName],offset,100);		\
			os_alarm_increment[ALARM_ID_##AlarmName]	=	offset;									\
			os_alarm_period[ALARM_ID_##AlarmName]	=	Cycle;										\
			assert(xResult == pdPASS);																\
	}while(0)
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32 		AppModeType;
typedef uint8 		StatusType;
typedef uint16 		AlarmType;
typedef TickType_t 	TickType;
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
extern EventGroupHandle_t os_task_events[TASK_NUM];
extern TaskHandle_t       os_task_handles [TASK_NUM ];
extern TimerHandle_t      os_alarm_handles[ALARM_NUM];
extern TickType           os_alarm_increment [ALARM_NUM];
extern TickType           os_alarm_period [ALARM_NUM];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void StartOS( AppModeType app_mode );
#endif /* COM_CLANG_INCLUDE_OS_H_ */
