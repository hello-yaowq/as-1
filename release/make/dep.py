#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2015  AS <parai@foxmail.com>
# *
# * This source code is free software; you can redistribute it and/or modify it
# * under the terms of the GNU General Public License version 2 as published by the
# * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
# *
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# * for more details.
# */
import os
import sys
import glob
import re


reAbs = re.compile(r'\[(.+)\]')

def get_abs(file):
    if(os.name == 'nt'):
        os.system('dir %s > .deps'%(file.replace('/','\\')))
        fp = open('.deps','r')
        cstr = ''
        for el in fp.readlines():
            cstr += el
        fp.close()
        grp = reAbs.search(cstr)
        if(grp):
            cstr = grp.groups()[0]
        else:
            cstr = os.path.abspath(file)
    else:
        os.system('readlink -f %s > .deps'%(file))
        fp = open('.deps','r')
        cstr = fp.readline()
        fp.close()
    
    return cstr.replace('\\','/')

def abs_dep(dep):
    dep = dep.replace('\n','')  
    dep = dep.replace('\\','') 
    grp = dep.split('.o:')
    cstr = '%s.o :'%(grp[0])
    deps = grp[1].split(' ')
    for d in deps:
        if(d != ''):
            cstr += ' %s'%(get_abs(d))
    return cstr
    

def Dep(obj_dir,dep):
    fd = open(dep,'w')
    for d in glob.glob('%s/*.d'%(obj_dir)):
        fp = open(d,'r')
        cstr = ''
        for el in fp.readlines():
            cstr += el
        fd.write('%s\n\n'%(abs_dep(cstr)))
        fp.close()
    src_dir = os.path.abspath('%s/../src'%(obj_dir))
    if(os.path.exists(src_dir)):
        for ss in glob.glob('%s/*.s'%(src_dir)):
            ss = os.path.basename(ss)[:-2]
            fd.write('%s\n\n'%(abs_dep('obj/%s.o:src/%s.s'%(ss,ss))))
    fd.close()
    print("  >> CC DEPC DOEN")
    os.system('rm .deps')

if(__name__ == '__main__'):
    if(len(sys.argv) == 3):
        Dep(sys.argv[1],sys.argv[2])
    else:
        print("usage:dep.py obj_dir target.dep")