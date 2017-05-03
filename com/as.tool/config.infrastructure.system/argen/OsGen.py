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
from .util import *
from .gen_freeosek import *
from .gen_toppers_osek import *
from .gen_smallos import *
from .gen_freertos import *
from .gen_toppers_atk2_sc4 import *
from .gen_contiki import *
from .gen_ucosii import *
from .gen_trampoline import *
import glob

__all__ = ['OsGen']

__osgen__ = {'freeosek':gen_freeosek,
             'toppers_osek':gen_toppers_osek,
             'smallos':gen_smallos,
             'freertos':gen_freertos,
             'atk2-sc4':gen_toppers_atk2_sc4,
             'contiki':gen_contiki,
             'posix':gen_toppers_osek,
             'ucosii':gen_ucosii,
             'trampoline':gen_trampolone
            }

def getOsRef(os_list):
    for each in os_list:
        if(each.tag == 'OsRef'):
            return each.attrib['name']
    # default os
    return 'toppers_osek'
    
def OsGen(gendir):
    os_list = ScanXML(gendir,'Os')
    os_ref = getOsRef(os_list)
    gen = __osgen__[os_ref]
    gen(gendir,os_list)
