"""/* Copyright(C) 2013, OpenSAR by Fan Wang(parai). All rights reserved.
 *
 * This file is part of OpenSAR.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Email: parai@foxmail.com
 * Source Open At: https://github.com/parai/OpenSAR/
 */
"""
import sys,os
from .GCF import *
from .util import *

__all__ = ['GenOS','gen_askar']

def GenOS(root,dir):
    global __dir
    GLInit(root)
    if(len(GLGet('TaskList')) == 0):return
    os_list = []
    for lst in root:
        for obj in lst:
            os_list.append(obj)
    gen_askar(dir,os_list)
    print('    >>> Gen OS DONE <<<')

def GenH(gendir,os_list):
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(GHeader('Os',Vendor='askar'))
    fp.write('#ifndef OS_CFG_H_\n#define OS_CFG_H_\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "kernel.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define __ASKAR_OS__\n\n')
    general = ScanFrom(os_list,'General')[0]
    if(GAGet(general,'ErrorHook') != 'NULL'):
        fp.write('#define OS_USE_ERROR_HOOK\n')
    try:
        cc = GAGet(general,'Conformance')
    except KeyError:
        cc = 'ECC2' # TODO: resolve conformance
    fp.write('#define OS_CONFORMANCE_CLASS %s\n'%(cc))
    fp.write('#define OS_STATUS %s\n'%(GAGet(general,'Status')))
    fp.write('\n\n')
    task_list = ScanFrom(os_list,'Task')
    maxPrio = 0
    multiPrio = False
    multiAct  = False
    sumAct = 0
    prioList=[]
    for id,task in enumerate(task_list):
        prio = Integer(GAGet(task,'Priority'))
        sumAct += Integer(GAGet(task,'Activation'))
        if(Integer(GAGet(task,'Activation')) > 1):
            multiAct = True;
        try:
            prioList.index(prio)
            multiPrio = True
        except ValueError:
            prioList.append(prio)
        if(prio > maxPrio):
            maxPrio = prio
    fp.write('#define PRIORITY_NUM %s\n'%(maxPrio))
    fp.write('#define ACTIVATION_SUM %s\n'%(sumAct+1))
    if(multiPrio):
        fp.write('#define MULTIPLY_TASK_PER_PRIORITY\n')
    if(multiAct):
        fp.write('#define MULTIPLY_TASK_ACTIVATION\n')
    fp.write('\n\n')
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
    
    counter_list = ScanFrom(os_list,'Counter')
    for id,counter in enumerate(counter_list):
        fp.write('#define COUNTER_ID_%-32s %s\n'%(GAGet(counter,'Name'),id))
    fp.write('#define COUNTER_NUM%-32s %s\n\n'%(' ',id+1))

    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('#define ALARM_ID_%-32s %s\n'%(GAGet(alarm,'Name'),id))
    fp.write('#define ALARM_NUM%-32s %s\n\n'%(' ',id+1))
    fp.write('\n\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    for id,task in enumerate(task_list):
        fp.write('extern TASK(%s);\n'%(GAGet(task,'Name')))
    fp.write('\n\n')
    for id,alarm in enumerate(alarm_list):
        fp.write('extern ALARM(%s);\n'%(GAGet(alarm,'Name')))
    fp.write('\n\n')
    fp.write('#endif /*OS_CFG_H_*/\n\n')
    fp.close()
    
def GenC(gendir,os_list):
    fp = open('%s/Os_Cfg.c'%(gendir),'w')
    fp.write(GHeader('Os',Vendor='askar'))
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "kernel_internal.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('static uint32_t %s_Stack[(%s*4+sizeof(uint32_t)-1)/sizeof(uint32_t)];\n'%(GAGet(task,'Name'),GAGet(task,'StackSize')))
    fp.write('const TaskConstType TaskConstArray[TASK_NUM] =\n{\n')
    for id,task in enumerate(task_list):
        runPrio = GAGet(task,'Priority')
        if(GAGet(task,'Schedule')=='NON'):
            runPrio = 'PRIORITY_NUM'
        maxAct = Integer(GAGet(task,'Activation'))
        if(len(GLGet(task,'EventList')) > 0):
            if(maxAct > 1):
                raise Exception('Task<%s>: multiple requesting of task activation allowed for basic tasks'%(GAGet(task,'Name')))
            maxAct = 1
        fp.write('\t{\n')
        fp.write('\t\t.pStack = %s_Stack,\n'%(GAGet(task,'Name')))
        fp.write('\t\t.stackSize = sizeof(%s_Stack),\n'%(GAGet(task,'Name')))
        fp.write('\t\t.entry = TaskMain%s,\n'%(GAGet(task,'Name')))
        fp.write('\t\t.initPriority = %s,\n'%(GAGet(task,'Priority')))
        fp.write('\t\t.runPriority = %s,\n'%(runPrio))
        fp.write('\t\t.name = "%s",\n'%(GAGet(task,'Name')))
        fp.write('\t\t#ifdef MULTIPLY_TASK_ACTIVATION\n')
        fp.write('\t\t.maxActivation = %s,\n'%(maxAct))
        fp.write('\t\t#endif\n')
        fp.write('\t\t.autoStart = %s,\n'%(GAGet(task,'Autostart').upper()))
        fp.write('\t},\n')
    fp.write('};\n\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    
    fp.close()

def gen_askar(gendir,os_list):
    GenH(gendir,os_list)
    GenC(gendir,os_list)
