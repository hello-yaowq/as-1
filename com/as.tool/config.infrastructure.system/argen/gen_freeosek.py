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

__all__ = ['gen_freeosek']

def toFreeOSEK_OIL(os_list,file):
    fp = open(file,'w')
    fp.write('OSEK OSEK {\n')
    fp.write('OS    ExampleOS {\n')
    fp.write('\tSTATUS = EXTENDED;\n')
    fp.write('\tPRETASKHOOK = FALSE;\n')
    fp.write('\tPOSTTASKHOOK = FALSE;\n')
    fp.write('\tSTARTUPHOOK = FALSE;\n')
    fp.write('\tERRORHOOK = FALSE;\n')
    fp.write('\tSHUTDOWNHOOK = FALSE;\n')
    fp.write('\tMEMMAP = FALSE;\n')
    fp.write('\tUSERESSCHEDULER = TRUE;\n')
    fp.write('};\n\n')
    task_list = ScanFrom(os_list,'Task')
    for id,task in enumerate(task_list):
        fp.write('TASK %s {\n'%(task.attrib['name']))
        fp.write('\tPRIORITY = %s;\n'%(task.attrib['priority']))
        fp.write('\tSCHEDULE = FULL;\n')
        fp.write('\tACTIVATION = %s;\n'%(task.attrib['max-activation']))
        if(task.attrib['auto-start']=='true'):
            fp.write('\tAUTOSTART = TRUE {\n')
            fp.write('\t\tAPPMODE = %s;\n'%(task.attrib['app-mode']))
            fp.write('\t};\n')
        else:
            fp.write('\tAUTOSTART = FALSE;\n')
        fp.write('\tSTACK = %s;\n'%(task.attrib['stack-size']))
        basic = True
        for mask,ev in enumerate(task):
            basic = False            
        if(basic): 
            fp.write('\tTYPE = BASIC;\n')
        else:
            fp.write('\tTYPE = EXTENDED;\n')
        for mask,ev in enumerate(task):
            fp.write('\tEVENT = %s;\n'%(ev.attrib['name']))
        fp.write('};\n\n')
    counter_list = ScanFrom(os_list,'Counter')
    for id,counter in enumerate(counter_list):
        fp.write('COUNTER %s {\n'%(counter.attrib['name']))
        fp.write('\tMAXALLOWEDVALUE = %s;\n'%(counter.attrib['max-value']))
        fp.write('\tTICKSPERBASE = %s;\n'%(counter.attrib['ticks-per-base']))
        fp.write('\tMINCYCLE = %s;\n'%(counter.attrib['min-value']))
        fp.write('\tTYPE = HARDWARE;\n')
        fp.write('\tCOUNTER = HWCOUNTER0;\n')
        fp.write('};\n\n')
    alarm_list = ScanFrom(os_list,'Alarm')
    for id,alarm in enumerate(alarm_list):
        fp.write('ALARM %s {\n'%(alarm.attrib['name']))
        fp.write('\tCOUNTER = %s;\n'%(alarm.attrib['counter']))
        if(alarm.attrib['auto-start']=='true'):
            fp.write('\tAUTOSTART = TRUE {\n')
            fp.write('\t\tAPPMODE = %s;\n'%(alarm.attrib['app-mode']))
            fp.write('\t\tALARMTIME = %s;\n'%(alarm.attrib['offset']))
            fp.write('\t\tCYCLETIME = %s;\n'%(alarm.attrib['period']))
            fp.write('\t};\n')
        else:
            fp.write('\tAUTOSTART = FALSE;\n')
        fp.write('\tACTION = ALARMCALLBACK {\n')
        fp.write('\t\tCALLBACK = %s;\n'%(alarm.attrib['name']))
        fp.write('\t};\n')
        fp.write('};\n\n')
    for id,task in enumerate(task_list):
        for mask,ev in enumerate(task):
            fp.write('EVENT %s;\n\n'%(ev.attrib['name']))
    fp.write('APPMODE OSDEFAULTAPPMODE;\n\n')
    fp.write('};\n\n')
    fp.close()
  
def gen_freeosek(gendir,os_list):
    toFreeOSEK_OIL(os_list,'%s/freeosek.oil'%(gendir))
    php_list=''
    for php in glob.glob('%s/*.php'%(gendir)):
        php_list += ' %s'%(php)
    cmd = 'php %s/OpenGEN/gen/generator.php  -v -c %s/freeosek.oil -f %s -o %s'%(gendir,gendir,php_list,gendir)
    print('  >> %s'%(cmd))
    os.system(cmd)
    fp = open('%s/Os_Cfg.h'%(gendir),'w')
    fp.write(__header)
    fp.write('#ifndef OS_CFG_H\n#define OS_CFG_H\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "osek_os.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define __FREEOSEK__\n')
    fp.write('#define HWCOUNTER0 0\n')
    fp.write('/* #define HWCOUNTER1 1 */\n\n')
    fp.write("#define OS_TICKS2MS(a) (a)\n\n")
#     task_list = ScanFrom(os_list,'Task')
#     for id,task in enumerate(task_list):
#         fp.write('#define TASK_ID_%-32s %s\n'%(task.attrib['name'],task.attrib['name']))
#     fp.write('\n\n')
#     alarm_list = ScanFrom(os_list,'Alarm')
#     for id,alarm in enumerate(alarm_list):
#         fp.write('#define ALARM_ID_%-32s %s\n'%(alarm.attrib['name'],alarm.attrib['name']))
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif /*OS_CFG_H*/\n\n')
    fp.close()