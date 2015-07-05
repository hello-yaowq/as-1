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
#ifndef OS_CFG_H
#define OS_CFG_H
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_OS_BASED_ON_FREERTOS

#define OSDEFAULTAPPMODE (0x00000001u)

#define  E_OS_ACCESS  (StatusType)1
#define  E_OS_CALLEVEL   (StatusType)2
#define  E_OS_ID         (StatusType)3
#define  E_OS_LIMIT      (StatusType)4
#define  E_OS_NOFUNC     (StatusType)5
#define  E_OS_RESOURCE   (StatusType)6
#define  E_OS_STATE      (StatusType)7
#define  E_OS_VALUE      (StatusType)8

#define OsWaitEvent_impl(ID,mask)                                \
     xEventGroupWaitBits (   os_task_events[ID],                 \
                             mask,                               \
                             pdFALSE,                            \
                             pdFALSE,                            \
                             portMAX_DELAY                       \
                         )

#define OsClearEvent_impl(ID,mask)       xEventGroupClearBits( os_task_events[ID], mask )
#define OsGetEvent_impl(ID)              xEventGroupClearBits( os_task_events[ID], 0 )
#define OsSetEvent_impl(ID,mask)         xEventGroupSetBits( os_task_events[ID], mask )
#define OsSetEventFromISR_impl(ID,mask)                              \
 do {                                                                \
     BaseType_t xHigherPriorityTaskWoken, xResult;                   \
     xHigherPriorityTaskWoken = pdFALSE;                             \
     xEventGroupSetBitsFromISR(  os_task_events[ID],                 \
                                 mask,                               \
                                 &xHigherPriorityTaskWoken);         \
     if( xResult == pdPASS )                                         \
     {                                                               \
         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );             \
     }                                                               \
 while(0)

/*
 * Task maximum activation is 1.
 */
#define OsActivateTask_impl(ID)  OsSetEvent_impl(ID,OS_EVENT_TASK_ACTIVATION)
/*
 * This will only clear the activation bit of the task.
 * Task will terminate itself when it returns from its main function TASK()
 */
#define OsTerminateTask_impl(ID)     OsClearEvent_impl(ID,OS_EVENT_TASK_ACTIVATION)

#define OsSetRelAlarm_impl(ID,Increment,Cycle)                                                   \
 do {                                                                                            \
         BaseType_t xResult;                                                                     \
         TickType offset;                                                                        \
         offset = Increment;                                                                     \
         if(offset > 0)                                                                          \
         {                                                                                       \
             offset = 1;                                                                         \
         }                                                                                       \
         xResult = xTimerChangePeriod(os_alarm_handles[ID],offset,100);                          \
         os_alarm_increment[ID]  =   offset;                                                     \
         os_alarm_period[ID] =   Cycle;                                                          \
         assert(xResult == pdPASS);                                                              \
 }while(0)
#define OsCancelAlarm_impl(ID)      xTimerStop(os_alarm_handles[ID],100)

#define OsWaitEvent(TaskName,mask)                   OsWaitEvent_impl(TASK_ID_##TaskName,mask)
#define OsClearEvent(TaskName,mask)                  OsClearEvent_impl(TASK_ID_##TaskName,mask)
#define OsGetEvent(TaskName)                         OsGetEvent_impl(TASK_ID_##TaskName)
#define OsSetEvent(TaskName,mask)                    OsSetEvent_impl(TASK_ID_##TaskName,mask)
#define OsSetEventFromISR(TaskName,mask)             OsSetEventFromISR_impl(TASK_ID_##TaskName,mask)

#define OsActivateTask(TaskName)                     OsActivateTask_impl(TASK_ID_##TaskName)
#define OsTerminateTask(TaskName)                    OsTerminateTask_impl(TASK_ID_##TaskName)

#define OsSetRelAlarm(AlarmName,Increment,Cycle)     OsSetRelAlarm_impl(ALARM_ID_##AlarmName,Increment,Cycle)
#define OsCancelAlarm(AlarmName)                     OsCancelAlarm_impl(ALARM_ID_##AlarmName)


#define TASK_ID_TaskApp                            0 
#define TASK_ID_TaskCom                            1 
#define TASK_ID_TaskSg                             2 
#define TASK_ID_TaskIdle                           3 
#define TASK_NUM  4 

#define TASK_PRIORITY_TaskApp                            5 
#define TASK_PRIORITY_TaskCom                            7 
#define TASK_PRIORITY_TaskSg                             8 
#define TASK_PRIORITY_TaskIdle                           1 

#define OS_EVENT_TASK_ACTIVATION                             ( 0x00800000u )
#define EVENT_MASK_TaskApp_Event1                            ( 1 << 0 )
#define EVENT_MASK_TaskApp_Event2                            ( 1 << 1 )
#define EVENT_MASK_TaskApp_Activation                        ( 0x00800000u ) /*reserved event for task activation */
#define EVENT_MASK_TaskCom_Activation                        ( 0x00800000u ) /*reserved event for task activation */
#define EVENT_MASK_TaskSg_Activation                         ( 0x00800000u ) /*reserved event for task activation */
#define EVENT_MASK_TaskIdle_Activation                       ( 0x00800000u ) /*reserved event for task activation */

#define TASK(TaskName)  void OsTaskMain##TaskName (void)

#define ALARM_ID_Alarm5ms                           0 
#define ALARM_ID_Alarm10ms                          1 
#define ALARM_ID_Alarm20ms                          2 
#define ALARM_ID_Alarm50ms                          3 
#define ALARM_ID_Alarm100ms                         4 
#define ALARM_NUM  5 

#define ALARM(AlarmName)  void OsAlarmMain##AlarmName (void)
/* ============================ [ TYPES     ] ====================================================== */
typedef uint32      AppModeType;
typedef uint8       StatusType;
typedef uint16      AlarmType;
typedef TickType_t  TickType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
extern EventGroupHandle_t os_task_events[TASK_NUM];
extern TaskHandle_t       os_task_handles [TASK_NUM ];
extern TimerHandle_t      os_alarm_handles[ALARM_NUM];
extern TickType           os_alarm_increment [ALARM_NUM];
extern TickType           os_alarm_period [ALARM_NUM];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern TASK(TaskApp);
extern TASK(TaskCom);
extern TASK(TaskSg);
extern TASK(TaskIdle);

extern ALARM(Alarm5ms);
extern ALARM(Alarm10ms);
extern ALARM(Alarm20ms);
extern ALARM(Alarm50ms);
extern ALARM(Alarm100ms);
#endif /* OS_CFG_H */

