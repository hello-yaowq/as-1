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
import os
import traceback

__all__=['ArGen','ArGenMain']
from argen.GenOS import GenOS
from argen.GenCan import GenCan
from argen.GenCanIf import GenCanIf
from argen.GenPduR import GenPduR
from argen.GenCanTp import GenCanTp
from argen.GenCom import GenCom
from argen.GenDcm import GenDcm
from argen.GenFls import GenFls
from argen.GenFee import GenFee
from argen.GenEa import GenEa
from argen.GenNvM import GenNvM

def dummy(arxml,dir):
    pass

__gen_engine =  {   
    'OS':GenOS,
    'Can':GenCan,'CanIf':GenCanIf,'CanTp':GenCanTp,'PduR':GenPduR,'Com':GenCom,'Dcm':GenDcm,
    'Fls':GenFls,'Fee':GenFee,'Ea':GenEa,'NvM':GenNvM,
    'EcuC':dummy
}

def ArGen(arxml,dir):
    if(arxml.tag == 'OS'):
        return # supress OS Gen
    try:
        engine = __gen_engine[arxml.tag]
    except KeyError:
        print('TODO: generator engine for "%s" is not implemented'%(arxml.tag))
        return
    engine(arxml,dir)

def ArGenMain(wfxml,gendir):
    import xml.etree.ElementTree as ET
    if(os.path.exists(wfxml)):
        root = ET.parse(wfxml).getroot();
        for arxml in root:
            ArGen(arxml,gendir)