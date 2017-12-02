'''/**
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
import sys,os
from .GCF import *

__all__ = ['GenDem']

__dir = '.'

def GenDem(root,dir):
    global __dir
    GLInit(root)
    __dir = '%s'%(dir)
    if(len(GLGet('EventClassList')) == 0):return
    GenH()
    GenC()
    print('    >>> Gen Dem DONE <<<')

def GenH():
    global __dir
    General= GLGet('General')
    fp = open('%s/Dem_Cfg.h'%(__dir),'w')
    fp.write(GHeader('Dem'))
    fp.write('#ifndef DEM_CFG_H\n#define DEM_CFG_H\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define DEM_VERSION_INFO_API        STD_%s\n'%(GAGet(General,'VersionInfoApi')))
    fp.write('#define DEM_DEV_ERROR_DETECT        STD_%s\n'%(GAGet(General,'DevelopmentErrorDetection')))
    fp.write('#define DEM_OBD_SUPPORT             STD_%s\n'%(GAGet(General,'DEM_OBD_SUPPORT')))
    fp.write('#define DEM_PTO_SUPPORT             STD_%s\n'%(GAGet(General,'DEM_PTO_SUPPORT')))
    fp.write('#define DEM_CLEAR_ALL_EVENTS        STD_%s\n\n'%(GAGet(General,'DEM_CLEAR_ALL_EVENTS')))
    fp.write('#define DEM_TYPE_OF_DTC_SUPPORTED          %s\n'%(GAGet(General,'DEM_TYPE_OF_DTC_SUPPORTED')))
    fp.write('#define DEM_DTC_STATUS_AVAILABILITY_MASK   %s\n'%(GAGet(General,'DEM_DTC_STATUS_AVAILABILITY_MASK')))
    fp.write('#define DEM_BSW_ERROR_BUFFER_SIZE          %s\n'%(GAGet(General,'DEM_BSW_ERROR_BUFFER_SIZE')))
    fp.write('''#define DEM_FF_DID_LENGTH                    TBD    // Length of DID & PID of FreezeFrames in Bytes.
#define DEM_MAX_NUMBER_EVENT_ENTRY_MIR        0    // Max nr of events stored in mirror memory.
#define DEM_MAX_NUMBER_EVENT_ENTRY_PER        0    // Max nr of events stored in permanent memory.
#define DEM_MAX_NUMBER_EVENT_ENTRY_PRI        10    // Max nr of events stored in primary memory.
#define DEM_MAX_NUMBER_EVENT_ENTRY_SEC        0    // Max nr of events stored in secondary memory.
#define DEM_MAX_NUMBER_PRESTORED_FF            0    // Max nr of prestored FreezeFrames. 0=Not supported.

/*
 * Size limitations of the types derived from DemGeneral
 */
#define DEM_MAX_NR_OF_RECORDS_IN_EXTENDED_DATA    10    // 0..253 according to Autosar
#define DEM_MAX_NR_OF_EVENT_DESTINATION             4    // 0..4 according to Autosar

/*
 * Size limitations of storage area
 */
#define DEM_MAX_SIZE_FF_DATA                     10    // Max number of bytes in one freeze frame
#define DEM_MAX_SIZE_EXT_DATA                     10    // Max number of bytes in one extended data record
#define DEM_MAX_NUMBER_EVENT                    100    // Max number of events to keep status on

#define DEM_MAX_NUMBER_EVENT_PRE_INIT             20    // Max number of events status to keep before init
#define DEM_MAX_NUMBER_FF_DATA_PRE_INIT             20    // Max number of freeze frames to store before init
#define DEM_MAX_NUMBER_EXT_DATA_PRE_INIT         20    // Max number of extended data to store before init

#define DEM_MAX_NUMBER_EVENT_PRI_MEM            (DEM_MAX_NUMBER_EVENT_ENTRY_PRI)    // Max number of events status to store in primary memory
#define DEM_MAX_NUMBER_FF_DATA_PRI_MEM            5                                    // Max number of freeze frames to store in primary memory
#define DEM_MAX_NUMBER_EXT_DATA_PRI_MEM            5                                    // Max number of extended data to store in primary memory

#define DEM_MAX_NUMBER_AGING_PRI_MEM 1
#define DEM_MAX_NR_OF_CLASSES_IN_FREEZEFRAME_DATA 1
#define DEM_MAX_NR_OF_RECORDS_IN_FREEZEFRAME_DATA 1
#define DEM_DID_IDENTIFIER_SIZE_OF_BYTES 1
#define DEM_FREEZEFRAME_DEFAULT_VALUE 1\n''')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif\n\n')
    EventParameterList= GLGet('EventParameterList')
    fp = open('%s/Dem_IntErrId.h'%(__dir),'w')
    fp.write(GHeader('Dem'))
    fp.write('#ifndef DEM_INTERRID_H\n#define DEM_INTERRID_H\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('''/*
 * Definition of event IDs used by BSW
 * NB! Must be unique for each event!
 */

enum {
    // Event IDs from DEM module
    DEM_EVENT_ID_NULL = 0,            // Do not change this entry!!!\n''')
    for evt in EventParameterList:
        if(GAGet(evt,'EventKind') == 'BSW'):
            fp.write('\t%s,\n'%(GAGet(evt,'Name')))
    fp.write('''    // DEM last event id for BSW
    DEM_EVENT_ID_LAST_FOR_BSW
};\n''')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif\n\n')

    fp = open('%s/Dem_IntEvtId.h'%(__dir),'w')
    fp.write(GHeader('Dem'))
    fp.write('#ifndef DEM_INTEVTID_H\n#define DEM_INTEVTID_H\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('''/*
 * Definition of event IDs used by SW-C
 * NB! Must be unique for each event!
 */
enum {
    // NB! Event IDs below DEM_SWC_EVENT_ID_START not allowed!
    DEM_EVENT_ID_SWC_START = DEM_EVENT_ID_LAST_FOR_BSW,\n''')
    for evt in EventParameterList:
        if(GAGet(evt,'EventKind') == 'SWC'):
            fp.write('\tDEM_EVENT_ID_%s,\n'%(GAGet(evt,'Name')))
    fp.write('};\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif\n\n')

def GenC():
    global __dir
    global __dir
    fp = open('%s/Dem_Cfg.c'%(__dir),'w')
    fp.write(GHeader('Dem'))
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Dem.h"\n\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    EventClassList= GLGet('EventClassList')
    for cst in EventClassList:
        fp.write('static const Dem_EventClassType EventClass_%s = \n{\n'%(GAGet(cst,'Name')))
        fp.write('\t.ConsiderPtoStatus=%s,\n'%(GAGet(cst,'ConsiderPtoStatus').upper()))
        fp.write('\t.EventPriority=%s,\n'%(GAGet(cst,'EventPriority')))
        fp.write('\t.FFPrestorageSupported=%s,\n'%(GAGet(cst,'FFPrestorageSupported').upper()))
        fp.write('\t.HealingAllowed=%s,\n'%(GAGet(cst,'HealingAllowed').upper()))
        fp.write('\t.OperationCycleRef=DEM_%s,\n'%(GAGet(cst,'OperationCycleRef')))
        fp.write('\t.HealingCycleRef=DEM_%s,\n'%(GAGet(cst,'HealingCycleRef')))
        fp.write('\t.HealingCycleCounter=%s,\n'%(GAGet(cst,'HealingCycleCounter')))
        fp.write('\t.ConfirmationCycleCounterThreshold=%s,\n'%(GAGet(cst,'ConfirmationCycleCounterThreshold')))
        fp.write('};\n\n')
    EventParameterList= GLGet('EventParameterList')
    fp.write('static const Dem_EventParameterType EventParameterList[] = \n{\n')
    for evt in EventParameterList:
        if(GAGet(evt,'EventKind') == 'BSW'):
            fp.write('\t{\n')
            fp.write('\t\t.EventID=%s,\n'%(GAGet(evt,'Name')))
            fp.write('\t\t.EventKind=DEM_EVENT_KIND_%s,\n'%(GAGet(evt,'EventKind')))
            fp.write('\t\t.EventClass=&EventClass_%s,\n'%(GAGet(evt,'EventClassRef')))
            fp.write('\t\t.Arc_EOL=FALSE,\n')
            fp.write('\t},\n')
    for evt in EventParameterList:
        if(GAGet(evt,'EventKind') == 'SWC'):
            fp.write('\t{\n')
            fp.write('\t\t.EventID=DEM_EVENT_ID_%s,\n'%(GAGet(evt,'Name')))
            fp.write('\t\t.EventKind=DEM_EVENT_KIND_%s,\n'%(GAGet(evt,'EventKind')))
            fp.write('\t\t.EventClass=&EventClass_%s,\n'%(GAGet(evt,'EventClassRef')))
            fp.write('\t\t.Arc_EOL=FALSE,\n')
            fp.write('\t},\n')
    fp.write('\t{\n\t\t.Arc_EOL=TRUE\n\t}\n};\n\n')
    fp.write('static const Dem_ConfigSetType DemConfig = \n{\n')
    fp.write('\t.EventParameter=EventParameterList,\n')
    fp.write('};\n\n')
    fp.write('const Dem_ConfigType DEM_Config = { .ConfigSet = &DemConfig };\n\n')
    fp.write('FreezeFrameRecType  FreezeFrameMirrorBuffer[DEM_MAX_NUMBER_FF_DATA_PRI_MEM];\n\n')
    fp.write('HealingRecType           HealingMirrorBuffer[DEM_MAX_NUMBER_AGING_PRI_MEM];\n\n')
    fp.write('const NvM_BlockIdType FreezeFrameBlockId[DEM_MAX_NUMBER_FF_DATA_PRI_MEM];\n\n')
    fp.write('const NvM_BlockIdType HealingBlockId;\n\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    

    fp.close()