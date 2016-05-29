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

__all__ = ['GenRte']

__dir = '.'

def GenRte(root,dir):
    global __dir
    GLInit(root)
    __dir = '%s'%(dir)
    if(len(GLGet('SwcList')) == 0):return
    GenH()
    GenC()
    print('    >>> Gen Rte DONE <<<')
    

def struct(s):
    s = s.strip()
    s = s.replace('{','{\n\t')
    s = s.replace(';',';\n\t') 
    s = s[:-2]+'}'
    return s
    

def GenRteTypesH():
    global __dir
    fp = open('%s/Rte_Types.h'%(__dir),'w')
    fp.write(GHeader('Rte_Types'))
    fp.write('#ifndef RTE_TYPES_H_\n#define RTE_TYPES_H_\n\n')
    fp.write('#include "Std_Types.h"\n\n')
    swcList = GLGet('SwcList')
    for swc in swcList:
        internalBehaviorList = GLGet(swc,'InternalBehaviorList')
        pimList = GLGet(internalBehaviorList,'PimList')
        for pim in pimList:
            fp.write('typedef %s Rte_PimType_%s_%s;\n\n'%(struct(GAGet(pim,'TypeDefinion')),GAGet(swc,'Name'),GAGet(pim,'TypeName')))
    
    fp.write('\n#endif\n\n');
    fp.close()
    

def GenSwcH(swc):
    global __dir
    fp = open('%s/Rte_%s.h'%(__dir,GAGet(swc,'Name')),'w')
    fp.write(GHeader('Rte_%s'%(GAGet(swc,'Name'))))
    fp.write('#ifndef RTE_%s_H_\n#define RTE_%s_H_\n\n'%(GAGet(swc,'Name'),GAGet(swc,'Name')))
    fp.write('#include "Rte_Types.h"\n\n')
    internalBehaviorList = GLGet(swc,'InternalBehaviorList')
    pimList = GLGet(internalBehaviorList,'PimList')
    for pim in pimList:
        fp.write('typedef Rte_PimType_%s_%s %s;\n\n'%(GAGet(swc,'Name'),GAGet(pim,'TypeName'),GAGet(pim,'TypeName')))
    
    fp.write('\n#endif\n\n');
    fp.close()

def GenH():
    global __dir
    GenRteTypesH()
    
    swcList = GLGet('SwcList')
    for swc in swcList:
        GenSwcH(swc)
    

def GenC():
    global __dir
    global __dir
    fp = open('%s/Rte.c'%(__dir),'w')
    fp.write(GHeader('Rte'))
    fp.write('#include "Rte_Types.h"\n\n')
    swcList = GLGet('SwcList')
    for swc in swcList:
        internalBehaviorList = GLGet(swc,'InternalBehaviorList')
        pimList = GLGet(internalBehaviorList,'PimList')
        for pim in pimList:
            fp.write('Rte_PimType_%s_%s %s;\n\n'%(GAGet(swc,'Name'),GAGet(pim,'TypeName'),GAGet(pim,'Name')))
    
    fp.close()
      