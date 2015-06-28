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
import os,sys
from KsmGen import *

__gen__ = [GenerateKSM]
def XCC(gendir):
    if(not os.path.exists(gendir)):os.mkdir(gendir)
    for g in __gen__:
        print('  %s ...'%(g.__name__))
        g(gendir)
    
if(__name__ == '__main__'):
    gendir = os.path.abspath(sys.argv[1])
    XCC(gendir)
    print('  >> XCC %s/*.xml done.'%(gendir))
    