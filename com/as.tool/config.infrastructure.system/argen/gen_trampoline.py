
__header = '''/**
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
'''

from .util import *
from .GCF import *

__all__ = ['gen_trampoline']

__tpl_customer_types__ = '''
#ifndef TPL_APP_CUSTOM_TYPES_H
#define TPL_APP_CUSTOM_TYPES_H

#include "tpl_compiler.h"
#include "tpl_os_std_types.h"

/**
 * tpl_heap_key is the datatype used to store the key of a heap entry.
 */
typedef uint8 tpl_heap_key;

/**
 * tpl_rank_count is the datatype used to store the rank of an activation
 * It is the lower part of a tpl_heap_key
 */
typedef uint8 tpl_rank_count;

/**
 * tpl_proc_id is used for task and isrs identifiers.
 *
 * @warning This data type must be signed
 * because -1 is used for INVALID_TASK and INVALID_ISR
 */
typedef sint8 tpl_proc_id;

/**
 * tpl_appmode_mask is used for AUTOSTART objects.
 *
 * Each bit in the mask corresponds to an application mode. The size of the mask
 * is computed according to the number of application modes.
 */
typedef uint8 tpl_appmode_mask;

/**
 * tpl_priority represents a task's or a resource's priority.
 *
 * @warning This type must be signed.
 */
typedef sint8 tpl_priority;

/**
 * tpl_activate_counter is used to count
 * how many time a task has been activated while
 * it was not suspended.
 */
typedef uint8 tpl_activate_counter;

/**
 * tpl_event_mask is used for event.
 * An event is a bit vector. According to the maximum number of events
 * defined in the application, it can be uint8 (8 events), uint16 (16 events) or
 * uint32 (32 events).
 */
typedef uint8 tpl_event_mask;

/**
 * tpl_alarm_type is used for alarm identifiers.
 */
typedef uint8 tpl_alarm_id;

/**
 * tpl_resource_id is used for resource identifiers.
 *
 * @warning RES_SCHEDULER is set to the higher resource id + 1
 */
typedef uint8 tpl_resource_id;

/**
 * @internal
 *
 * Type used to store the id of an alarm or a schedule table
 * This is used only for tracing
 */
typedef uint8 tpl_timeobj_id;

/**
 * @internal
 *
 * @todo: document this
 */
typedef uint8 tpl_counter_id;

/**
 * @internal
 *
 * @todo: document this
 */
typedef uint8 tpl_schedtable_id;

/**
 * @internal
 *
 * Type used to store the id of an OS Application
 * Its size is computed according to the number of OS Applications
 */
typedef uint8 tpl_app_id;

/**
 * @internal
 *
 * Generic type used in OS Application API
 * Its size is computed by doing the max of all object kind
 */
typedef uint8 tpl_generic_id;

typedef uint32 tpl_ioc_id;

#endif /* TPL_APP_CUSTOM_TYPES_H */

/* End of file tpl_app_custom_types.h */

'''

def Trampoline_TaskList(os_list):
    ret_list = []
    task_list = ScanFrom(os_list,'Task')
    for task in task_list:
        length = len(ret_list)
        if(length == 0):
            ret_list.append(task)
        else:
            prio = int(GAGet(task,'Priority'))
            flag = False
            for i,it in enumerate(ret_list):
                iprio = int(GAGet(it,'Priority'))
                if(prio < iprio):
                    ret_list.insert(i, task)
                    flag = True
                    break
            if(flag == False):
                ret_list.append(task)
    return ret_list

def genForTrampolineMisc_H(gendir,os_list):
    fp = open('%s/tpl_app_define.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef TPL_APP_DEFINE_H\n#define TPL_APP_DEFINE_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "tpl_config_def.h"\n\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('''#define TARGET_ARCH                      "posix"
#define TARGET_CHIP                      ""
#define TARGET_BOARD                     ""
#define NUMBER_OF_CORES                  1
#define WITH_OS_EXTENDED                 YES
#define WITH_ORTI                        NO
#define WITH_PAINT_STACK                 YES
#define WITH_PAINT_REGISTERS             YES
#define WITH_STARTUP_HOOK                YES
#define WITH_SHUTDOWN_HOOK               YES
#define WITH_ERROR_HOOK                  YES
#define WITH_PRE_TASK_HOOK               YES
#define WITH_POST_TASK_HOOK              YES
#define WITH_PANIC_HOOK                  YES
#define WITH_USEGETSERVICEID             NO
#define WITH_USEPARAMETERACCESS          NO
#define WITH_USERESSCHEDULER             YES
#define WITH_SYSTEM_CALL                 NO
#define WITH_MEMORY_PROTECTION           YES
#define WITH_MEMMAP                      YES
#define WITH_COMPILER_SETTINGS           NO
#define WITH_AUTOSAR                     YES
#define WITH_PROTECTION_HOOK             YES
#define WITH_STACK_MONITORING            YES
#define WITH_AUTOSAR_TIMING_PROTECTION   YES
#define AUTOSAR_SC                       0
#define WITH_OSAPPLICATION               YES
#define WITH_OSAPPLICATION_STARTUP_HOOK  YES
#define WITH_OSAPPLICATION_SHUTDOWN_HOOK YES
#define WITH_TRACE                       YES
#define WITH_IT_TABLE                    NO
#define WITH_COM                         YES
#define WITH_IOC                         YES
#define WITH_MODULES_INIT                NO
#define WITH_INIT_BOARD                  NO
#define WITH_ISR2_PRIORITY_MASKING       NO

/* Defines related to the key part of a ready list entry.
 * The key part has in the most significant bits the priority of the job and
 * in the least significant bits the rank of the job. So:
 * - PRIORITY_SHIFT is the number of bit the key has to be shifted to the
 *   right) to get the priority only aligned to the right;
 * - PRIORITY_MASK is the mask to get the priority only (not aligned to the
 *   right with the rank set to 0;
 * - RANK_MASK is the mask to get only the rank.
 */
#define PRIORITY_SHIFT                   1
#define PRIORITY_MASK                    6
#define RANK_MASK                        1\n\n''');
    task_list = Trampoline_TaskList(os_list)
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

    res_list = ScanFrom(os_list, 'Resource')
    for id,res in enumerate(res_list):
        fp.write('#define RES_ID_%-32s %s\n'%(GAGet(res,'Name'),id+1))
    fp.write('#define RES_NUMBER %s\n\n'%(len(res_list)+1))
    
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(GAGet(alarm,'Name'),id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))

    fp.write('#define TASK_COUNT TASK_NUM\n')
    fp.write('#define EXTENDED_TASK_COUNT TASK_NUM\n')
    fp.write('#define ALARM_COUNT ALARM_NUM\n')
    fp.write('#define RESOURCE_COUNT RES_NUMBER\n')

    fp.write('#define COUNTER_COUNT 1\n')
    fp.write('#define SCHEDTABLE_COUNT 0\n')
    fp.write('#define IOC_COUNT 0\n')
    fp.write('#define ISR_COUNT 0\n')
    fp.write('#define APP_COUNT 1\n')
    fp.write('#define NUMBER_OF_OBJECTS 0\n')
    fp.write('#define RES_SCHEDULER_PRIORITY 3\n')
    fp.write('#define IDLE_TASK_ID    TASK_COUNT + ISR_COUNT\n')

    fp.write('\n#define TRACE_FORMAT() tpl_trace_format_txt();\n')
    fp.write('#define TRACE_FILE "trampoline_os_trace.txt"\n')
    fp.write('\n\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('\n\n')
    fp.write('#endif /* TPL_APP_DEFINE_H */\n\n')
    fp.close()

    fp = open('%s/tpl_app_custom_types.h'%(gendir),'w')
    fp.write(__tpl_customer_types__)
    fp.close()

    fp = open('%s/tpl_app_config.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef TPL_APP_CONFIG_H\n#define TPL_APP_CONFIG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "tpl_app_define.h"\n\n')
    fp.write('#include "tpl_compiler.h"\n\n')
    fp.write('#include "tpl_os_internal_types.h"\n\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif /* TPL_APP_CONFIG_H */\n')
    fp.close()

def genForTrampoline_H(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "tpl_os.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define __TRAMPOLINE_OS__\n\n')
    fp.write('#define ALARM ALARMCALLBACK\n\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('DeclareResource(RES_SCHEDULER);\n\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('\n\n')
    fp.write('#endif /* OS_CFG_H */\n\n')
    fp.close()
def genForTrampoline_C(gendir,os_list):
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
    task_list = Trampoline_TaskList(os_list)
    
    alarm_list = ScanFrom(os_list,'Alarm')

    fp.close()
def gen_trampoline(gendir,os_list):
    genForTrampolineMisc_H(gendir,os_list)
    genForTrampoline_H(gendir,os_list)
    genForTrampoline_C(gendir,os_list)

