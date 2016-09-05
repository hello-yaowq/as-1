""" * AS - the open source Automotive Software on https://github.com/parai
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
"""
import sys,os
from .GCF import *

__all__ = ['GenXcp']

__dir = '.'
    
def GenXcp(root,dir):
    global __dir,__root
    GLInit(root)
    if(len(GLGet('XcpDaqList')) == 0):return
    __dir = '%s'%(dir)
    GenH()
    GenC()
    print("    >>> Gen Xcp DONE <<<")

def GenH():
    global __dir
    fp = open('%s/Xcp_Cfg.h'%(__dir),'w')
    fp.write(GHeader('XCP'))
    General=GLGet('General')
    
    fp.write('#ifndef XCP_CFG_H\n#define XCP_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "Xcp_ConfigTypes.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define XCP_DEV_ERROR_DETECT STD_%s\n'%(GAGet(General,'DevelopmentErrorDetection')))
    fp.write('#define XCP_VERSION_INFO_API STD_%s\n'%(GAGet(General,'VersionInfoApi')))
    fp.write('#define XCP_FEATURE_GET_SLAVE_ID STD_ON\n')
    
    fp.write('#define XCP_IDENTIFICATION XCP_IDENTIFICATION_%s\n'%(GAGet(General,'XcpIdentificationFieldType')))
    fp.write('#define XCP_DAQ_COUNT  %s\n'%(GAGet(General,'XcpDaqCount')))
    fp.write('#define XCP_ODT_COUNT  %s\n'%(GAGet(General,'XcpOdtCount')))
    fp.write('#define XCP_ODT_ENTRIES_COUNT %s\n'%(GAGet(General,'XcpOdtEntriesCount')))
    fp.write('#define XCP_MAX_CTO %s\n'%(GAGet(General,'XcpMaxCto')))
    fp.write('#define XCP_MAX_DTO %s\n'%(GAGet(General,'XcpMaxDto')))
    fp.write('#define XCP_MAX_RXTX_QUEUE %s\n'%(GAGet(General,'XcpDaqCount')))
    fp.write('#define XCP_PROTOCOL XCP_PROTOCOL_CAN\n')
    fp.write('#define XCP_PDU_ID_TX 0\n')
    fp.write('#define XCP_PDU_ID_RX 0\n')
    fp.write('#define XCP_CAN_ID_RX 0\n')
    fp.write('#define XCP_PDU_ID_BROADCAST 0\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif\n')
    fp.close()
    
    fp = open('%s/XcpOnCan_Cfg.h'%(__dir),'w')
    fp.write(GHeader('XCP'))
    fp.write('#ifndef XCP_ON_CAN_CFG_H\n#define XCP_ON_CAN_CFG_H\n\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif\n')
    fp.close()

def GenC():
    global __dir
    fp = open('%s/Xcp_Cfg.c'%(__dir),'w')
    fp.write(GHeader('XCP'))
    fp.close()
