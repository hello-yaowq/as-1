'''/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
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

__all__ = ['GenJ1939Tp']

__dir = '.'

def GenJ1939Tp(root,dir):
    global __dir
    GLInit(root)
    __dir = '%s'%(dir)
    if(len(GLGet('ChannelList')) == 0):return
    GenH()
    GenC()
    print('    >>> Gen J1939Tp DONE <<<')

def GenH():
    global __dir
    General= GLGet('General')
    fp = open('%s/J1939Tp_Cfg.h'%(__dir),'w')
    fp.write(GHeader('J1939Tp'))
    fp.write('#ifndef J1939TP_CFG_H\n#define J1939TP_CFG_H\n')
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "SchM_cfg.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('#define J1939TP_VERSION_INFO_API        STD_%s\n'%(GAGet(General,'VersionInfoApi')))
    fp.write('#define J1939TP_DEV_ERROR_DETECT        STD_%s\n'%(GAGet(General,'DevelopmentErrorDetection')))
    fp.write('#define J1939TP_MAIN_FUNCTION_PERIOD MAIN_FUNCTION_PERIOD(J1939TP)\n')
    ChannelList = GLGet('ChannelList')
    rxList = []
    txList = []
    for chl in ChannelList:
        if(GAGet(chl,'Direction') == 'RX'):
            rxList.append((chl))
        else:
            txList.append((chl))
    fp.write('#define J1939TP_TX_CHANNEL_COUNT %s\n'%(len(txList)))
    fp.write('#define J1939TP_RX_CHANNEL_COUNT %s\n'%(len(rxList)))
    fp.write('#define J1939TP_TX_CONF_TIMEOUT 1\n')
    fp.write('#define J1939TP_RX_PDU_COUNT 1\n')
    fp.write('#define J1939TP_PACKETS_PER_BLOCK 1\n')
    fp.write('#define J1939TP_PG_COUNT 1\n')
    fp.write('#define J1939TP_CHANNEL_COUNT (J1939TP_TX_CHANNEL_COUNT+J1939TP_RX_CHANNEL_COUNT)\n')
    fp.write('#define J1939TP_ID_J1939TP_RX 0\n')
    fp.write('#define J1939TP_ID_J1939TP_TX 0\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.write('#endif /* J1939TP_CFG_H */\n')
    fp.close();

def GenC():
    global __dir
    fp = open('%s/J1939Tp_Cfg.c'%(__dir),'w')
    fp.write(GHeader('J1939Tp'))
    fp.write('/* ============================ [ INCLUDES  ] ====================================================== */\n')
    fp.write('#include "J1939Tp.h"\n')
    fp.write('/* ============================ [ MACROS    ] ====================================================== */\n')
    fp.write('/* ============================ [ TYPES     ] ====================================================== */\n')
    fp.write('/* ============================ [ DECLARES  ] ====================================================== */\n')
    fp.write('/* ============================ [ DATAS     ] ====================================================== */\n')
    fp.write('const J1939Tp_ConfigType J1939Tp_Config =\n{\n')
    fp.write('};\n')
    fp.write('/* ============================ [ LOCALS    ] ====================================================== */\n')
    fp.write('/* ============================ [ FUNCTIONS ] ====================================================== */\n')
    fp.close();