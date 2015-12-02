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
from .GCF import *

__all__ = ['GenEa']

__dir = '.'

def GenEa(root,dir):
    global __dir
    GLInit(root)
    __dir = '%s'%(dir)
    if(len(GLGet('BlockList')) == 0):return
    GenH()
    GenC()
    print('    >>> Gen Ea DONE <<<')
    
def GenH():
    global __dir
    fp = open('%s/Ea_Cfg.h'%(__dir),'w')
    fp.write(GHeader('Ea'))
    General=GLGet('General')
    BlockList = GLGet('BlockList')
    cstr = ''
    for id,block in enumerate(BlockList):
        cstr += '#define EA_BLOCK_ID_%-32s %s\n'%(GAGet(block,'Name').upper(),id)
    cstr += '#define EA_NUM_OF_BLOCKS  %s\n'%(len(BlockList))
    fp.write('''#ifndef EA_CFG_H_
#define EA_CFG_H_

#include "MemIf_Types.h"

#define EA_DEV_ERROR_DETECT            STD_%s
#define EA_VERSION_INFO_API            STD_%s
#define EA_SET_MODE_SUPPORTED          STD_%s

#define EA_INDEX                        (0xEA)
#define EA_VIRTUAL_PAGE_SIZE            %s

%s
/*
    ITEM NAME:        <Ea_BlockConfigType>
    SCOPE:            <Ea Module>
    DESCRIPTION:
        Configuration of block specific parameters for the EEPROM abstraction module.
*/
/*@req EA040
*/

typedef struct {
    /*@req EA116
    */
    /*Block identifier (handle). 0x0000 and 0xFFFF shall not be used for block numbers (see EA006).
    Range:    min = 2^NVM_DATA_SELECTION_BITS
            max = 0xFFFF -2^NVM_DATA_SELECTION_BITS
    Note:     Depending on the number of bits set aside for dataset selection several other block numbers shall
            also be left out to ease implementation.
    NVM_DATA_SELECTION_BITS = 4        So range: 0x10 ~ 0xFFEF*/
    uint16    EaBlockNumber;

    /*Size of a logical block in bytes.*/
    /*@req EA117
    */
    uint16    EaBlockSize;

    /*Marker for high priority data.
    true: Block contains immediate data.
    false: Block does not contain immediate data.*/
    /*@req EA118
    */
    boolean    EaImmediateData;

    /*Device index (handle). Range: 0 .. 254 (0xFF reserved for broadcast call to GetStatus function).
    Type:    Reference to EepGeneral item 'EepDriverIndex'
    dependency: This information is needed by the NVRAM manager respectively the Memory Abstraction
    Interface to address a certain logical block. It is listed in this specification to give a complete overview
    over all block related configuration parameters.*/
    /*@req EA115
    */
    uint8    EaDeviceIndex;

    /*indicates the end of the Ea Block List*/
    boolean EaBlockEOL;
}Ea_BlockConfigType;

/*
    ITEM NAME:        <Ea_GeneralType>
    SCOPE:            <Ea Module>
    DESCRIPTION:
        General configuration of the EEPROM abstraction module. This container lists block independent
        configuration parameters.
*/
typedef struct {
    /*Specifies the InstanceId of this module instance. If only one instance is present it shall have the Id 0.*/
    uint8    EaIndex;

    /*Mapped to the job end notification routine provided by the upper layer module (NvM_JobEndNotification).*/
    void (*EaNvmJobEndNotification)(void);

    /*Mapped to the job error notification routine provided by the upper layer module (NvM_JobErrorNotification). */
    void (*EaNvmJobErrorNotification)(void);
}Ea_GeneralType;

extern const Ea_GeneralType Ea_GeneralData;
extern const Ea_BlockConfigType Ea_BlockConfigData[];

#endif /*EA_CFG_H_*/\n'''%(GAGet(General,'DevelopmentErrorDetection'),
                  GAGet(General,'VersionInfoApi'),
                  GAGet(General,'SetModeApi'),
                  GAGet(General,'VirtualPageSize'),
                  cstr))
    fp.close()
    
def GenC():
    global __dir
    fp = open('%s/Ea_Cfg.c'%(__dir),'w')
    fp.write(GHeader('Ea'))
    General=GLGet('General')
    BlockList = GLGet('BlockList')
    fp.write('#include "Ea.h"\n#include "NvM_Cbk.h"\n\n')
    if(GAGet(General,'NvmJobEndNotification') != 'NULL'):
        fp.write('extern void %s(void);\n'%(GAGet(General,'NvmJobEndNotification')))
    if(GAGet(General,'NvmJobErrorNotification') != 'NULL'):
        fp.write('extern void %s(void);\n'%(GAGet(General,'NvmJobErrorNotification')))  
    fp.write('''\nconst Ea_GeneralType Ea_GeneralData = {
    .EaIndex = EA_INDEX,
    .EaNvmJobEndNotification = %s,
    .EaNvmJobErrorNotification = %s,
};\n\n'''%(GAGet(General,'NvmJobEndNotification'),GAGet(General,'NvmJobErrorNotification')))
     
    fp.write('\nconst Ea_BlockConfigType Ea_BlockConfigData[] = {\n')     
    for id,block in enumerate(BlockList):
        fp.write('\t{    /* %s */\n'%(GAGet(block,'Name')))
        fp.write('\t\t.EaBlockNumber = EA_BLOCK_ID_%s,\n'%(GAGet(block,'Name').upper()))
        fp.write('\t\t.EaBlockSize  =  %s,\n'%(GAGet(block,'BlockSize')))
        fp.write('\t\t.EaImmediateData = %s,\n'%(GAGet(block,'ImmediateData').upper()))
        fp.write('\t\t.EaDeviceIndex = EA_INDEX,\n') 
        if(id+1 == len(BlockList)): 
            fp.write('\t\t.EaBlockEOL = TRUE\n')
        else:
            fp.write('\t\t.EaBlockEOL = FALSE\n')
        fp.write('\t},\n') 
    fp.write('};\n\n')  
    fp.close()    
