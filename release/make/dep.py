import os
import sys
import glob
import re


reAbs = re.compile(r'\[(.+)\]')

def get_abs(file):
    if(os.name == 'nt'):
        os.system('dir %s > .deps'%(file.replace('/','\\')))
        fp = open('.deps','r')
        cstr = fp.readlines()[5]
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
    dep = dep.replace('\r','') 
    dep = dep.replace('\\','') 
    grp = dep.split(':')
    cstr = '%s :'%(grp[0])
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
        for el in fp.readline():
            cstr += el
        fd.write('%s\n\n'%(abs_dep(cstr)))
        fp.close()
    fd.close()

if(__name__ == '__main__'):
    if(len(sys.argv) == 3):
        Dep(sys.argv[1],sys.argv[2])
    else:
        print("usage:dep.py obj_dir target.dep")