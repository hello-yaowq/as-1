__lic__ = '''
/**
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

def merge(s1,s2,so):
    fo = open(so,'w')
    fp = open(s1)
    for el in fp.readlines():
        if(el[:2] != 'S7'):
            fo.write(el)
    fp.close()
    fp = open(s2)
    for el in fp.readlines():
        if(el[:2] != 'S0'):
            fo.write(el)
    fp.close()    
    fo.close()


if(__name__=='__main__'):
    if(len(sys.argv)==6 and sys.argv[1]=='-m' and sys.argv[4]=='-o'):
        merge(sys.argv[2],sys.argv[3],sys.argv[5])
    else:
        print('Usage: %s -m first.s19 sencond.s19 -o first_second.s19'%(sys.argv[0]))
