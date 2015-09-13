__header = '''/**
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
'''

import sys,os
from util import *

__all__ = ['OsGen']

__for_toppers_osek_macro =  \
'''#define ALARM(AlarmName)  ALARMCALLBACK(AlarmName)

/* OSEK OS Debugh helper */
#define LOG_STAOS_ENTER(mode)
#define LOG_STAOS_LEAVE()
#define LOG_TERTSK_ENTER()
#define LOG_TERTSK_LEAVE(ercd)
#define LOG_STUTOS_ENTER(ercd)
#define LOG_STUTOS_LEAVE()
#define LOG_ACTTSK_ENTER(tskid)
#define LOG_ACTTSK_LEAVE(ercd)
#define LOG_SIGCNT_ENTER(cntid)
#define LOG_SIGCNT_LEAVE(ercd)
#define LOG_SETREL_ENTER(almid, incr, cycle)
#define LOG_SETREL_LEAVE(ercd)
#define LOG_SETABS_ENTER(almid, incr, cycle)
#define LOG_SETABS_LEAVE(ercd)
#define LOG_SETEVT_ENTER(tskid, mask)
#define LOG_SETEVT_LEAVE(ercd)
#define LOG_CLREVT_ENTER(mask)
#define LOG_CLREVT_LEAVE(ercd)
#define LOG_GETEVT_ENTER(tskid, p_mask)
#define LOG_GETEVT_LEAVE(ercd, mask)
#define LOG_WAIEVT_ENTER(mask)
#define LOG_WAIEVT_LEAVE(ercd)
#define LOG_GETRES_ENTER(resid)
#define LOG_GETRES_LEAVE(ercd)
#define LOG_RELRES_ENTER(resid)
#define LOG_RELRES_LEAVE(ercd)
#define LOG_GETALB_ENTER(almid, p_info)
#define LOG_GETALB_LEAVE(ercd)
#define LOG_GETALM_ENTER(almid, p_tick)
#define LOG_GETALM_LEAVE(ercd)
#define LOG_CANALM_ENTER(almid)
#define LOG_CANALM_LEAVE(ercd)
#define LOG_DISINT_ENTER()
#define LOG_DISINT_LEAVE()
#define LOG_ENAINT_ENTER()
#define LOG_ENAINT_LEAVE()
#define LOG_SUSALL_ENTER()
#define LOG_SUSALL_LEAVE()
#define LOG_RSMALL_ENTER()
#define LOG_RSMALL_LEAVE()
#define LOG_SUSOSI_ENTER()
#define LOG_SUSOSI_LEAVE()
#define LOG_RSMOSI_ENTER()
#define LOG_RSMOSI_LEAVE()
#define LOG_GETAAM_ENTER()
#define LOG_GETAAM_LEAVE(mode)
#define LOG_CHNTSK_ENTER(tskid)
#define LOG_CHNTSK_LEAVE(ercd)
#define LOG_SCHED_ENTER()
#define LOG_SCHED_LEAVE(ercd)
#define LOG_GETTID_ENTER(p_tskid)
#define LOG_GETTID_LEAVE(ercd, p_tskid)
#define LOG_GETTST_ENTER(tskid, p_state)
#define LOG_GETTST_LEAVE(ercd,  p_state)'''

__for_freertos_macros = \
"""
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

#define OsWaitEvent_impl(ID,mask)                                \\
     xEventGroupWaitBits (   os_task_events[ID],                 \\
                             mask,                               \\
                             pdFALSE,                            \\
                             pdFALSE,                            \\
                             portMAX_DELAY                       \\
                         )

#define OsClearEvent_impl(ID,mask)       xEventGroupClearBits( os_task_events[ID], mask )
#define OsGetEvent_impl(ID)              xEventGroupClearBits( os_task_events[ID], 0 )
#define OsSetEvent_impl(ID,mask)         xEventGroupSetBits( os_task_events[ID], mask )
#define OsSetEventFromISR_impl(ID,mask)                              \\
 do {                                                                \\
     BaseType_t xHigherPriorityTaskWoken, xResult;                   \\
     xHigherPriorityTaskWoken = pdFALSE;                             \\
     xEventGroupSetBitsFromISR(  os_task_events[ID],                 \\
                                 mask,                               \\
                                 &xHigherPriorityTaskWoken);         \\
     if( xResult == pdPASS )                                         \\
     {                                                               \\
         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );             \\
     }                                                               \\
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

#define OsSetRelAlarm_impl(ID,Increment,Cycle)                                                   \\
 do {                                                                                            \\
         BaseType_t xResult;                                                                     \\
         TickType offset;                                                                        \\
         offset = Increment;                                                                     \\
         if(offset > 0)                                                                          \\
         {                                                                                       \\
             offset = 1;                                                                         \\
         }                                                                                       \\
         xResult = xTimerChangePeriod(os_alarm_handles[ID],offset,100);                          \\
         os_alarm_increment[ID]  =   offset;                                                     \\
         os_alarm_period[ID] =   Cycle;                                                          \\
         assert(xResult == pdPASS);                                                              \\
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

#define SetRelAlarm OsSetRelAlarm_impl
#define CancelAlarm OsCancelAlarm_impl

#define GetResource(r)
#define ReleaseResource(r)
"""

__for_freertos_functions = \
'''/* ============================ [ LOCALS    ] ====================================================== */
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
%s
}

void OsAlarmInit(void)
{
    uint32 i;
    for(i=0;i<ALARM_NUM;i++)
    {
        os_alarm_handles[i] = xTimerCreate(os_alarm_names[i],100,pdTRUE,(void*)i,FreeRTOS_AlarmProcess);
        assert(os_alarm_handles[i]!=NULL);
    }
%s
}

void StartOS( AppModeType app_mode )
{
    OsTaskInit();
    OsAlarmInit();

    StartupHook();

    vTaskStartScheduler();
}

void ShutdownOS(StatusType ercd)
{
    assert(0);
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
#ifdef __WINDOWS__
    _assert(__func__, pcFileName, ulLine);
#else
	__assert(__func__, pcFileName, ulLine);
#endif
}

unsigned long ulGetRunTimeCounterValue( void )
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{

}
'''

def genForToppersOSEK_H(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Std_Types.h"\n')
    fp.write('#include "kernel.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define AS_OS_BASED_ON_TOPPERS_OSEK\n\n')
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('#define TASK_ID_%-32s %s\n'%(task.attrib['name'],id))
    fp.write('#define TASK_NUM%-32s %s\n\n'%(' ',id+1))
    for id,task in enumerate(task_list):
        fp.write('#define TASK_PRIORITY_%-32s %s\n'%(task.attrib['name'],task.attrib['priority']))
    fp.write('\n')
    for id,task in enumerate(task_list):
        for mask,ev in enumerate(task):
            if(ev.attrib['mask']=='auto'):
                mask = '(1<<%s)'%(mask)
            else:
                mask = ev.attrib['mask']
            fp.write('#define EVENT_MASK_%-40s %s\n'%('%s_%s'%(task.attrib['name'],ev.attrib['name']),mask))
    fp.write('\n')
    
    isr_list = ScanFrom(os_list,'ISR')
    isr_num = len(isr_list)
    for isr in isr_list:
        if((int(isr.attrib['vector'],10)+1)>isr_num):
            isr_num = int(isr.attrib['vector'],10)+1
    fp.write('#define ISR_NUM  %s\n\n'%(isr_num))
    
    counter_list = ScanFrom(os_list,'Counter')
    for id,counter in enumerate(counter_list):
        fp.write('#define COUNTER_ID_%-32s %s\n'%(counter.attrib['name'],id))
    fp.write('#define COUNTER_NUM%-32s %s\n\n'%(' ',id+1))
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(alarm.attrib['name'],id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))
    
    fp.write('%s\n'%(__for_toppers_osek_macro))
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    for id,task in enumerate(task_list):
        fp.write('extern TASK(%s);\n'%(task.attrib['name']))
    fp.write('\n\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('extern ALARM(%s);\n'%(alarm.attrib['name']))
    fp.write('\n\n')
    fp.write('#endif /* OS_CFG_H */\n\n')
    fp.close()

def genForToppersOSEK_C(gendir,os_list):
    fp = open('%s/Os_Cfg.c'%(gendir),'w')
    fp.write(__header)
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Os.h"\n')
    fp.write('#include "osek_kernel.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('typedef void (*void_function_void_t)(void);\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    isr_list = ScanFrom(os_list,'ISR')
    for isr in isr_list:
        fp.write('extern void %s (void);\n'%(isr.attrib['name']))
    fp.write('extern void task_initialize(void);\n')
    fp.write('extern void alarm_initialize(void);\n')
    fp.write('extern void resource_initialize(void);\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('const UINT8 tnum_task    = TASK_NUM;\n')
    fp.write('const UINT8 tnum_exttask = TASK_NUM;\n')
    fp.write('\n')
    task_list = ScanFrom(os_list,'Task')
    fp.write('const Priority  tinib_inipri[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s, /* %s */\n'%(task.attrib['priority'],task.attrib['name']))
    fp.write('};\n\n')
    fp.write('const Priority  tinib_exepri[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s, /* %s */\n'%(task.attrib['exe-priority'],task.attrib['name']))
    fp.write('};\n\n')
    fp.write('const UINT8  tinib_maxact[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s, /* %s */\n'%(task.attrib['max-activation'],task.attrib['name']))
    fp.write('};\n\n')
    fp.write('const AppModeType  tinib_autoact[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s, /* %s */\n'%(task.attrib['app-mode'],task.attrib['name']))
    fp.write('};\n\n')
    fp.write('const FP  tinib_task[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\tTASKNAME(%s),\n'%(task.attrib['name']))
    fp.write('};\n\n')
    for id,task in enumerate(task_list):
        fp.write('static UINT8  %s_stk[ %s ];\n'%(task.attrib['name'],task.attrib['stack-size']))
    fp.write('const VP  tinib_stk[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s_stk,\n'%(task.attrib['name']))
    fp.write('};\n\n')
    fp.write('const UINT16  tinib_stksz[TASK_NUM] = {\n')
    for id,task in enumerate(task_list):
        fp.write('\t%s,/* %s */\n'%(task.attrib['stack-size'],task.attrib['name']))
    fp.write('};\n\n')
    
    fp.write('const IPL       ipl_maxisr2 = 1;\n')
    fp.write('/*\n')
    fp.write(' *  os_cfg.c\n')
    fp.write(' *\n')
    fp.write(' *  used to manage tasks < tasks queue, state, proority, activate count >\n')
    fp.write(' */\n')
    fp.write('TaskType        tcb_next[TASK_NUM];         /* task linker,indicate the next task in the queue */\n')
    fp.write('UINT8           tcb_tstat[TASK_NUM];        /* task state */\n')
    fp.write('Priority        tcb_curpri[TASK_NUM];       /* task current priority */\n')
    fp.write('UINT8           tcb_actcnt[TASK_NUM];       /* task current activate count */\n')
    fp.write('EventMaskType   tcb_curevt[TASK_NUM];       /* task current event has been set */\n')
    fp.write('EventMaskType   tcb_waievt[TASK_NUM];       /* task wait event */\n')
    fp.write('ResourceType    tcb_lastres[TASK_NUM];      /* task acquired resource recently */\n')
    fp.write('/*\n')
    fp.write(' *  number of ALARM and Counter\n')
    fp.write(' */\n')
    fp.write('const UINT8     tnum_counter = COUNTER_NUM;                /* Counter number configured */\n')
    fp.write('const UINT8     tnum_alarm = ALARM_NUM;          /* Alarm number configured */\n')
    fp.write('/*\n')
    fp.write(' *  configured information for counter\n')
    fp.write(' */\n')
    counter_list = ScanFrom(os_list,'Counter')
    fp.write('const TickType  cntinib_maxval[COUNTER_NUM] = {\n')
    for id,counter in enumerate(counter_list):
        fp.write('\t%s,/* %s */\n'%(counter.attrib['max-value'],counter.attrib['name']))
    fp.write('};\n\n')
    fp.write('const TickType  cntinib_maxval2[COUNTER_NUM] = {\n')
    for id,counter in enumerate(counter_list):
        fp.write('\t%s*2+1,/* %s */\n'%(counter.attrib['max-value'],counter.attrib['name']))
    fp.write('};\n\n')
    fp.write('const TickType  cntinib_mincyc[COUNTER_NUM] = {\n')
    for id,counter in enumerate(counter_list):
        fp.write('\t%s,/* %s */\n'%(counter.attrib['min-value'],counter.attrib['name']))
    fp.write('};\n\n')
    fp.write('const TickType  cntinib_tickbase[COUNTER_NUM] = {\n')
    for id,counter in enumerate(counter_list):
        fp.write('\t%s,/* %s */\n'%(counter.attrib['ticks-per-base'],counter.attrib['name']))
    fp.write('};\n\n')
    fp.write('/*\n')
    fp.write(' *  counter control block\n')
    fp.write(' */\n')
    fp.write('TickType            cntcb_curval[COUNTER_NUM];      /* counter current value*/\n')
    fp.write('AlarmType           cntcb_almque[COUNTER_NUM];      /* counter queue */\n')
    fp.write('/*\n')
    fp.write(' *  configured information for alarm\n')
    fp.write(' */\n')
    alarm_list = ScanFrom(os_list,'Alarm')
    fp.write('const CounterType alminib_cntid[ALARM_NUM] = {\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\tCOUNTER_ID_%s, /* %s */\n'%(alarm.attrib['counter'],alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('const FP             alminib_cback[ALARM_NUM] = {\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\tALARMCALLBACKNAME(%s),\n'%(alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('const AppModeType alminib_autosta[ALARM_NUM] = {\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\t%s, /* %s */\n'%(alarm.attrib['app-mode'],alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('const TickType   alminib_almval[ALARM_NUM] = {\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\t%s, /* %s */\n'%(alarm.attrib['offset'],alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('const TickType   alminib_cycle[ALARM_NUM] = {\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\t%s, /* %s */\n'%(alarm.attrib['period'],alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('/*\n')
    fp.write(' *  alarm control block\n')
    fp.write(' */\n')
    fp.write('AlarmType       almcb_next[ALARM_NUM];      /* next alarm in queue */\n')
    fp.write('AlarmType       almcb_prev[ALARM_NUM];      /* previous alarm in queue */\n')
    fp.write('TickType        almcb_almval[ALARM_NUM];    /* expire time */\n')
    fp.write('TickType        almcb_cycle[ALARM_NUM];     /* alarm cycle time */\n')
    fp.write('\n')
    fp.write('const UINT8     tnum_resource = 1;\n')
    fp.write('const Priority  resinib_ceilpri[1] =\n')
    fp.write('{\n')
    fp.write('    15,\n')
    fp.write('};\n')
    fp.write('/*\n')
    fp.write(' *  resource control block\n')
    fp.write(' *\n')
    fp.write(' */\n')
    fp.write('Priority            rescb_prevpri[1];    /* previous priority of task which has acquired this res */\n')
    fp.write('ResourceType        rescb_prevres[1];\n')
    fp.write('\n')
    fp.write('const UINT8     tnum_isr2 = 0;\n')
    fp.write('const Priority  isrinib_intpri[] =\n')
    fp.write('{\n')
    fp.write('    0\n')
    fp.write('};\n')
    fp.write('ResourceType        isrcb_lastres[1];\n\n')
    isr_num = len(isr_list)
    for isr in isr_list:
        if((int(isr.attrib['vector'],10)+1)>isr_num):
            isr_num = int(isr.attrib['vector'],10)+1
    if(isr_num > 0):
        fp.write('const FP tisr_pc[ %s ] = {\n'%(isr_num))
        for iid in range(isr_num):
            iname = 'NULL'
            for isr in isr_list:
                if(iid == int(isr.attrib['vector'])):
                    iname = isr.attrib['name']
                    break
            fp.write('\t%s, /* %s */\n'%(iname,iid))
        fp.write('};\n\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('void object_initialize(void)\n')
    fp.write('{\n')
    fp.write('    task_initialize();\n')
    fp.write('    alarm_initialize();\n')
    fp.write('    resource_initialize();\n')
    fp.write('}\n')
    fp.close()
    
def genForToppersOSEK(gendir,os_list):
    genForToppersOSEK_H(gendir,os_list)
    genForToppersOSEK_C(gendir,os_list)

def genForFreeRTOS_H(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Std_Types.h"\n')
    fp.write('#include "FreeRTOS.h"\n')
    fp.write('#include "task.h"\n')
    fp.write('#include "semphr.h"\n')
    fp.write('#include "event_groups.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('%s\n'%(__for_freertos_macros))
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('#define TASK_ID_%-32s %s\n'%(task.attrib['name'],id))
    fp.write('#define TASK_NUM%-32s %s\n\n'%(' ',id+1))
    for id,task in enumerate(task_list):
        fp.write('#define TASK_PRIORITY_%-32s %s\n'%(task.attrib['name'],task.attrib['priority']))
    fp.write('\n')
    fp.write('#define OS_EVENT_TASK_ACTIVATION                            ( 0x00800000u )\n')
    for id,task in enumerate(task_list):
        for mask,ev in enumerate(task):
            if(ev.attrib['mask']=='auto'):
                mask = '(1<<%s)'%(mask)
            else:
                mask = ev.attrib['mask']
            fp.write('#define EVENT_MASK_%-40s %s\n'%('%s_%s'%(task.attrib['name'],ev.attrib['name']),mask))
    fp.write('\n')
    fp.write('#define TASK(TaskName)  void OsTaskMain##TaskName (void)\n')
    fp.write('\n')
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(alarm.attrib['name'],id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))
    fp.write('#define ALARM(AlarmName)  void OsAlarmMain##AlarmName (void)\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('typedef uint32      AppModeType;\n')
    fp.write('typedef uint8       StatusType;\n')
    fp.write('typedef uint16      AlarmType;\n')
    fp.write('typedef TickType_t  TickType;\n')
    fp.write('\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')    
    fp.write('extern EventGroupHandle_t os_task_events[TASK_NUM];\n')
    fp.write('extern TaskHandle_t       os_task_handles [TASK_NUM ];\n')
    fp.write('extern TimerHandle_t      os_alarm_handles[ALARM_NUM];\n')
    fp.write('extern TickType           os_alarm_increment [ALARM_NUM];\n')
    fp.write('extern TickType           os_alarm_period [ALARM_NUM];\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    for id,task in enumerate(task_list):
        fp.write('extern TASK(%s);\n'%(task.attrib['name']))
    fp.write('\n\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('extern ALARM(%s);\n'%(alarm.attrib['name']))
    fp.write('\n\n')
    fp.write('#endif /* OS_CFG_H */\n')
    fp.close()

def genForFreeRTOS_C(gendir,os_list):
    fp = open('%s/Os_Cfg.c'%(gendir),'w')
    fp.write(__header)
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Os.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('typedef void (*void_function_void_t)(void);\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('EventGroupHandle_t os_task_events  [TASK_NUM ];\n')
    fp.write('TaskHandle_t       os_task_handles [TASK_NUM ];\n')
    fp.write('TimerHandle_t      os_alarm_handles[ALARM_NUM];\n')
    fp.write('TickType           os_alarm_increment [ALARM_NUM];\n')
    fp.write('TickType           os_alarm_period [ALARM_NUM];\n')
    task_list = ScanFrom(os_list,'Task')
    alarm_list = ScanFrom(os_list,'Alarm')
    fp.write('static const void_function_void_t os_task_entrys[TASK_NUM] = { \n')
    for id,task in enumerate(task_list):
        fp.write('\tOsTaskMain%s,\n'%(task.attrib['name']))
    fp.write('};\n\n')
    fp.write('static const char os_task_names[TASK_NUM][32] = { \n')
    for id,task in enumerate(task_list):
        fp.write('\t"%s",\n'%(task.attrib['name']))
    fp.write('};\n\n')   
    fp.write('static const uint16 os_task_stack_size[TASK_NUM] = { \n')
    for id,task in enumerate(task_list):
        fp.write('\t%s,\n'%(task.attrib['stack-size']))
    fp.write('};\n\n')  
    fp.write('static const uint8 os_task_prioritys[TASK_NUM] = { \n')
    for id,task in enumerate(task_list):
        fp.write('\t%s,\n'%(task.attrib['priority']))
    fp.write('};\n\n')  
    fp.write('static const void_function_void_t os_alarm_entrys[ALARM_NUM] = { \n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\tOsAlarmMain%s,\n'%(alarm.attrib['name']))
    fp.write('};\n\n')
    fp.write('static const char os_alarm_names[ALARM_NUM][32] = { \n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\t"%s",\n'%(alarm.attrib['name']))
    fp.write('};\n\n')  
    str_activate_task = ''
    for id,task in enumerate(task_list):
        if(task.attrib['auto-start']=='true'):
            str_activate_task += '\tOsActivateTask(%s);\n'%(task.attrib['name'])
    str_active_alarm  = ''
    for id,alarm in enumerate(alarm_list):
        if(alarm.attrib['auto-start']=='true'):
            str_active_alarm += '\tOsSetRelAlarm(%s,%s,%s);\n'%(alarm.attrib['name'],alarm.attrib['offset'],alarm.attrib['period'])    
    fp.write(__for_freertos_functions%(str_activate_task,str_active_alarm))           
    fp.close()

def genForFreeRTOS(gendir,os_list):
    genForFreeRTOS_H(gendir,os_list)
    genForFreeRTOS_C(gendir,os_list)
    
def getOsRef(os_list):
    for each in os_list:
        if(each.tag == 'OsRef'):
            return each.attrib['name']
    # default os
    return 'toppers_osek'
    
def OsGen(gendir):
    os_list = ScanXML(gendir,'Os')
    os_ref = getOsRef(os_list)
    if(os_ref=='toppers_osek'):
        genForToppersOSEK(gendir,os_list)
    elif(os_ref=='freertos'):
        genForFreeRTOS(gendir,os_list)
    else:
        assert(0)
