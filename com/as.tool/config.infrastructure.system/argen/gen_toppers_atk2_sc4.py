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

__all__ = ['gen_toppers_atk2_sc4']

def genH(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Std_Types.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define __TOPPERS_ATK2_SC4__\n\n')
    fp.write('#define ALLFUNC\n\n')
    fp.write('#define RES_SCHEDULER 0 \n\n')
    fp.write('#define OSDEFAULTAPPMODE 0 \n\n')
    fp.write('#define CFG_USE_ERRORHOOK\n')
    fp.write('#define CFG_USE_POSTTASKHOOK\n')
    fp.write('#define CFG_USE_PRETASKHOOK\n')
    fp.write('#define CFG_USE_STACKMONITORING\n')
    fp.write('#define CFG_USE_PROTECTIONHOOK\n')
    fp.write('#define CFG_USE_SHUTDOWNHOOK\n')
    fp.write('\n')
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('#define TASK_ID_%-32s %s\n'%(GAGet(task,'Name'),id))
    fp.write('#define TASK_NUM%-32s %s\n\n'%(' ',id+1))
    for id,task in enumerate(task_list):
        fp.write('#define TASK_PRIORITY_%-32s %s\n'%(GAGet(task,'Name'),GAGet(task,'Priority')))
    fp.write('\n')
    for id,task in enumerate(task_list):
        for mask,ev in enumerate(GLGet(task,'EventList')):
            if(GAGet(ev,'Mask')=='AUTO'):
                mask = '(1<<%s)'%(mask)
            else:
                mask = GAGet(ev,'Mask')
            fp.write('#define EVENT_MASK_%-40s %s\n'%('%s_%s'%(GAGet(task,'Name'),GAGet(ev,'Name')),mask))
    fp.write('\n')
    isr_list = ScanFrom(os_list,'ISR')
    isr_num = len(isr_list)
    for isr in isr_list:
        if((int(isr.attrib['vector'],10)+1)>isr_num):
            isr_num = int(isr.attrib['vector'],10)+1
    fp.write('#define ISR_NUM  %s\n\n'%(isr_num))
    
    counter_list = ScanFrom(os_list,'Counter')
    for id,counter in enumerate(counter_list):
        fp.write('#define COUNTER_ID_%-32s %s\n'%(GAGet(counter,'Name'),id))
    fp.write('#define COUNTER_NUM%-32s %s\n\n'%(' ',id+1))
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(GAGet(alarm,'Name'),id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))
    fp.write('\n#define ALARM(a)  void AlarmMain##a(void)\n\n')
    
    fp.write('#include "atk_os.h"\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('extern void object_initialize(void);\n')
    fp.write('extern void object_terminate(void);\n\n')
    for id,task in enumerate(task_list):
        fp.write('extern TASK(%s);\n'%(GAGet(task,'Name')))
    fp.write('\n\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('extern ALARM(%s);\n'%(GAGet(alarm,'Name')))
    fp.write('\n#endif /* OS_CFG_H_ */\n\n')
    fp.close()

def genC(gendir,os_list):
    fp = open('%s/Os_Cfg.c'%(gendir),'w')
    fp.write(__header)
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Std_Types.h"\n')
    fp.write('#include "Os.h"\n')
    fp.write('#include "kernel_impl.h"\n')
    fp.write('#include "osap.h"\n')
    fp.write('#include "counter.h"\n')
    fp.write('#include "interrupt.h"\n')
    fp.write('#include "scheduletable.h"\n')
    fp.write('#include "task.h"\n')
    fp.write('#include "alarm.h"\n')
    fp.write('#include "ioc_impl.h"\n')
    fp.write('#include "memory.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('typedef void (*void_function_void_t)(void);\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    isr_list = ScanFrom(os_list,'ISR')
    for isr in isr_list:
        fp.write('extern void %s (void);\n'%(isr.attrib['name']))
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('const TaskType tnum_task    = TASK_NUM;\n')
    fp.write('const TaskType tnum_exttask = TASK_NUM;\n')
    fp.write('TCB            *p_runtsk;\n')
    fp.write('TCB            *p_schedtsk;\n')
    fp.write('PriorityType    nextpri;\n')
    fp.write('QUEUE        ready_queue[TNUM_TPRI];\n')
    fp.write('uint16        ready_primap;\n')
    fp.write('TCB            tcb_table[TASK_NUM];\n')
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('static StackType  %s_stk[ (%s+sizeof(StackType)-1)/sizeof(StackType) ];\n'%(GAGet(task,'Name'),GAGet(task,'StackSize')))
    fp.write('const TINIB    tinib_table[TASK_NUM] = \n{\n')
    for id,task in enumerate(task_list):
        fp.write('\t{ /* %s */\n'%(GAGet(task,'Name')))
        fp.write('\t\t.task=TaskMain%s,\n'%(GAGet(task,'Name')))
        fp.write('\t\t.sstksz=%s,\n'%(GAGet(task,'StackSize')))
        fp.write('\t\t.sstk=(void*)%s_stk,\n'%(GAGet(task,'Name')))
        fp.write('\t\t.ustksz=%s, /* TODO: ?? */\n'%(GAGet(task,'StackSize')))
        fp.write('\t\t.ustk=(void*)%s_stk, /* TODO: ?? */\n'%(GAGet(task,'Name')))
        fp.write('\t\t.p_osapcb=NULL,\n')
        fp.write('\t\t.acsbtmp=0,\n')
        fp.write('\t\t.inipri=%s,\n'%(GAGet(task,'Priority')))
        fp.write('\t\t.exepri=%s,\n'%(GAGet(task,'Priority')))
        fp.write('\t\t.maxact=%s,\n'%(GAGet(task,'Activation')))
        if (GAGet(task,'Autostart').upper() == 'TRUE'):
            fp.write('\t\t.autoact=%s,\n'%('OSDEFAULTAPPMODE'))
        else:
            fp.write('\t\t.autoact=%s,\n'%('0'))
        fp.write('\t\t.time_frame.tfcount=%s,\n'%('0'))
        fp.write('\t\t.time_frame.tftick=%s,\n'%('0'))
        fp.write('\t\t.execution_budget=%s,\n'%('1'))
        fp.write('\t\t.monitoring=%s,\n'%('1'))
        fp.write('\t},\n')
    fp.write('};\n\n')
    fp.write('const AppModeType    tnum_appmode = 1;\n')
    fp.write('\n')
    fp.write('const InterruptNumberType    tnum_intno = 0;\n')
    fp.write('const INTINIB                intinib_table[1];\n')
    fp.write('\n')
    fp.write('const CounterType    tnum_hardcounter=0;\n')
    fp.write('const CounterType    tnum_counter=0;\n')
    fp.write('const CNTINIB        cntinib_table[1];\n')
    fp.write('CNTCB                cntcb_table[1];\n')
    fp.write('const HWCNTINIB        hwcntinib_table[1];\n')
    fp.write('\n')
    fp.write('const ISRType    tnum_isr2=0;\n')
    fp.write('const ISRINIB    isrinib_table[1];\n')
    fp.write('ISRCB            isrcb_table[1];\n')
    fp.write('ISRCB            *p_runisr;\n')
    fp.write('uint8            sus_all_cnt;\n')
    fp.write('uint8            sus_os_cnt;\n')
    fp.write('PriorityType        sus_os_prevpri;\n')
    fp.write('const TickType    osinterruptlock_budget;\n')
    fp.write('TickType            os_difftime;\n')
    fp.write('MonitoringType    os_saved_watchtype;\n')
    fp.write('\n')
    fp.write('const ScheduleTableType    tnum_scheduletable=0;\n')
    fp.write('const ScheduleTableType    tnum_implscheduletable=0;\n')
    fp.write('const SCHTBLINIB            schtblinib_table[1];\n')
    fp.write('SCHTBLCB                    schtblcb_table[1];\n')
    fp.write('\n')
    fp.write('\n')
    fp.write('const ApplicationType            tnum_osap=0;\n')
    fp.write('const TrustedFunctionIndexType    tnum_tfn=0;\n')
    fp.write('OSAPCB            osapcb_table[1];\n')
    fp.write('OSAPCB            *p_runosap;\n')
    fp.write('const OSAPINIB    osapinib_table[1];\n')
    fp.write('const TFINIB        tfinib_table[1];\n')
    fp.write('\n')
    fp.write('\n')
    fp.write('const AlarmType    tnum_alarm=0;\n')
    fp.write('const ALMINIB    alminib_table[1];\n')
    fp.write('ALMCB            almcb_table[1];\n')
    fp.write('\n')
    fp.write('const IocType    tnum_ioc=0;\n')
    fp.write('const IocType    tnum_queueioc=0;\n')
    fp.write('const IOCWRPINIB    iocwrpinib_table[1];\n')
    fp.write('const IOCINIB    iocinib_table[1];\n')
    fp.write('IOCCB            ioccb_table[1];\n')
    fp.write('const IOCWRPINIB    iocwrpinib_table[1];\n')
    fp.write('void                *ioc_inival_table[1];\n')
    fp.write('\n')
    fp.write('const ResourceType    tnum_stdresource=0;\n')
    fp.write('const RESINIB        resinib_table[1];\n')
    fp.write('RESCB                rescb_table[1];\n')
    fp.write('\n')
    fp.write('const uint32        tnum_meminib=0;\n')
    fp.write('void * const    memtop_table[1];\n')
    fp.write('const MEMINIB    meminib_table[1];\n')
    fp.write('\n')
    fp.write('const uint32            tnum_datasec=0;\n')
    fp.write('const DATASECINIB    datasecinib_table[1];\n')
    fp.write('const uint32        tnum_bsssec=0;\n')
    fp.write('const BSSSECINIB    bsssecinib_table[1];\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('void object_initialize(void)\n')
    fp.write('{\n')
    fp.write('\ttask_initialize();\n')
    fp.write('\talarm_initialize();\n')
    fp.write('\tresource_initialize();\n')
    fp.write('}\n')
    fp.write('void object_terminate(void)  {}\n')
    fp.write('\n')

def gen_toppers_atk2_sc4(gendir,os_list):
    genH(gendir,os_list)
    genC(gendir,os_list)