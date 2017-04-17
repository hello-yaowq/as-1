
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

from .util import *
from .GCF import *

__all__ = ['gen_ucosii']

__for_ucosii_macros = \
"""
#define __UCOSII_OS__

#define OsWaitEvent_impl(ID,mask))
#define OsClearEvent_impl(ID,mask) 
#define OsGetEvent_impl(ID)      
#define OsSetEvent_impl(ID,mask)
#define OsSetEventFromISR_impl(ID,mask)

/*
 * Task maximum activation is 1.
 */
#define OsActivateTask_impl(ID)
/*
 * This will only clear the activation bit of the task.
 * Task will terminate itself when it returns from its main function TASK()
 */
#define OsTerminateTask_impl(ID)

#define OsSetRelAlarm_impl(ID,Increment,Cycle)

#define OsCancelAlarm_impl(ID)

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

#define GetTaskID(t) OsGetTaskID_impl(t)

#define ActivateTask OsActivateTask_impl
#define SetEvent     OsSetEvent_impl
#define WaitEvent(mask)
#define GetEvent(__tid,pmask)
#define ClearEvent(mask)

#define GetResource(r)
#define ReleaseResource(r)

#define DisableAllInterrupts
#define EnableAllInterrupts

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
"""

def uCOSII_TaskList(os_list):
    ret_list = []
    task_list = ScanFrom(os_list,'Task')
    for task in task_list:
        length = len(ret_list)
        if(length == 0):
            ret_list.append(task)
        else:
            prio = int(GAGet(task,'Priority'))
            flag = False
            for it in ret_list:
                iprio = int(GAGet(it,'Priority'))
                if(prio < iprio):
                    ret_list.insert(0, task)
                    flag = True
                    break
            if(flag == False):
                ret_list.append(task)
    return ret_list
    
def genForuCOSII_H(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Std_Types.h"\n')
    fp.write('#include "Os.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('%s\n\n'%(__for_ucosii_macros))
    task_list = uCOSII_TaskList(os_list)
    for id,task in enumerate(task_list):
        fp.write('#define TASK_ID_%-32s %-3s /* priority = %s */\n'%(GAGet(task,'Name'),id,GAGet(task,'Priority')))
    fp.write('#define TASK_NUM%-32s %s\n\n'%(' ',id+1))
    for id,task in enumerate(task_list):
        for mask,ev in enumerate(GLGet(task,'EventList')):
            if(GAGet(ev,'Mask')=='auto'):
                mask = '(1<<%s)'%(mask)
            else:
                mask = GAGet(ev,'Mask')
            fp.write('#define EVENT_MASK_%-40s %s\n'%('%s_%s'%(GAGet(task,'Name'),GAGet(ev,'Name')),mask))
    fp.write('\n')
    
    res_list = ScanFrom(os_list, 'Resource')
    for id,res in enumerate(res_list):
        fp.write('#define RES_ID_%-32s %s\n'%(GAGet(res,'Name'),id+1))
    fp.write('#define RES_NUMBER %s\n\n'%(len(res_list)+1))
    
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(GAGet(alarm,'Name'),id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))
    fp.write('\n\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('''typedef uint8 					StatusType;
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
}alarm_declare_t;\n\n''')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    if(len(task_list) > 0):
        fp.write('extern CONST(task_declare_t,AUTOMATIC)  TaskList[TASK_NUM];\n')
    if(len(alarm_list) > 0):
        fp.write('extern CONST(alarm_declare_t,AUTOMATIC) AlarmList[ALARM_NUM];\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    for id,task in enumerate(task_list):
        fp.write('extern TASK(%s);\n'%(GAGet(task,'Name')))
    fp.write('\n\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('extern ALARM(%s);\n'%(GAGet(alarm,'Name')))
    fp.write('\n\n')
    fp.write('#endif /* OS_CFG_H */\n\n')
    fp.close()
def genForuCOSII_C(gendir,os_list):
    fp = open('%s/Os_Cfg.c'%(gendir),'w')
    fp.write(__header)
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Os.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    task_list = uCOSII_TaskList(os_list)
    fp.write('CONST(task_declare_t,AUTOMATIC)  TaskList[TASK_NUM] = \n{\n')
    for id,task in enumerate(task_list):
        fp.write('\tDeclareTask(%-32s, %-5s, %s    ),\n'%(GAGet(task,'Name'),GAGet(task,'Autostart').upper(),'OSDEFAULTAPPMODE'))
    fp.write('};\n\n')
    
    alarm_list = ScanFrom(os_list,'Alarm')
    fp.write('CONST(alarm_declare_t,AUTOMATIC) AlarmList[ALARM_NUM] = \n{\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('\tDeclareAlarm(%s),\n'%(GAGet(alarm,'Name')))
    fp.write('};\n\n')
    fp.write('\n\n')
    fp.close()
def gen_ucosii(gendir,os_list):
    genForuCOSII_H(gendir,os_list)
    genForuCOSII_C(gendir,os_list)
