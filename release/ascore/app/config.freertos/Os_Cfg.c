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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Os.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef void (*void_function_void_t)(void);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
EventGroupHandle_t os_task_events  [TASK_NUM ];
TaskHandle_t       os_task_handles [TASK_NUM ];
TimerHandle_t      os_alarm_handles[ALARM_NUM];
TickType           os_alarm_increment [ALARM_NUM];
TickType           os_alarm_period [ALARM_NUM];
static const void_function_void_t os_task_entrys[TASK_NUM] = 
{
    OsTaskMainTaskApp,
    OsTaskMainTaskCom,
    OsTaskMainTaskSg,
    OsTaskMainTaskIdle,
};
static const char os_task_names[TASK_NUM][32] = 
{
    "TaskApp",
    "TaskCom",
    "TaskSg",
    "TaskIdle",
};
static const uint16 os_task_stack_size[TASK_NUM]= 
{
     512 ,
     512 ,
     512 ,
     512 ,
};
static const uint8 os_task_prioritys[TASK_NUM] = 
{
     5 ,
     7 ,
     8 ,
     1 ,
};
static const void_function_void_t os_alarm_entrys[ALARM_NUM] = 
{
    OsAlarmMainAlarm5ms,
    OsAlarmMainAlarm10ms,
    OsAlarmMainAlarm20ms,
    OsAlarmMainAlarm50ms,
    OsAlarmMainAlarm100ms,
};
static const char os_alarm_names[ALARM_NUM][32] = 
{
    "Alarm5ms",
    "Alarm10ms",
    "Alarm20ms",
    "Alarm50ms",
    "Alarm100ms",
};
/* ============================ [ LOCALS    ] ====================================================== */
static void FreeRTOS_TaskProcess(void* pvParameters)
{
    for( ;; )
    {
        OsWaitEvent_impl((uint32)pvParameters, OS_EVENT_TASK_ACTIVATION);
        os_task_entrys[(uint32)pvParameters]();
    }
}
static void FreeRTOS_AlarmProcess(TimerHandle_t xTimer)
{
    uint32 id = (uint32)pvTimerGetTimerID(xTimer);

    os_alarm_entrys[id]();

    if(os_alarm_increment[id]!=0)
    {
        os_alarm_increment[id] = 0;
        if(0u == os_alarm_period[id])
        {
            OsCancelAlarm_impl(id);
        }
        else
        {
            xTimerChangePeriod(os_alarm_handles[id],os_alarm_period[id] ,100);
        }
    }
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void OsTaskInit(void)
{
    uint32 i;
    for(i=0;i<TASK_NUM;i++)
    {
        os_task_events[i] = xEventGroupCreate();
        assert(os_task_events[i]!=NULL);
        xTaskCreate( FreeRTOS_TaskProcess,os_task_names[i], os_task_stack_size[i], (void*)i, os_task_prioritys[i], &os_task_handles[i] );
        assert(os_task_handles[i]!=NULL);
    }
    /* Task Automatically activation */
    OsActivateTask(TaskApp);
    OsActivateTask(TaskCom);
    OsActivateTask(TaskSg);
    OsActivateTask(TaskIdle);
}

void OsAlarmInit(void)
{
    uint32 i;
    for(i=0;i<ALARM_NUM;i++)
    {
        os_alarm_handles[i] = xTimerCreate(os_alarm_names[i],100,pdTRUE,(void*)i,FreeRTOS_AlarmProcess);
        assert(os_alarm_handles[i]!=NULL);
    }
    OsSetRelAlarm(Alarm5ms, 5 , 5 );
    OsSetRelAlarm(Alarm10ms, 6 , 10 );
    OsSetRelAlarm(Alarm20ms, 7 , 20 );
    OsSetRelAlarm(Alarm50ms, 8 , 50 );
    OsSetRelAlarm(Alarm100ms, 9 , 100 );
}

void StartOS( AppModeType app_mode )
{
    OsTaskInit();
    OsAlarmInit();

    StartupHook();

    vTaskStartScheduler();
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    _assert(__FUNCTION__, pcFileName, ulLine);
}

unsigned long ulGetRunTimeCounterValue( void )
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{

}

