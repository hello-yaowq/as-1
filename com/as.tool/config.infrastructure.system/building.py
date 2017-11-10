import os
import shutil
from SCons.Script import *

Env = None

BuildOptions = {}

def PrepareRTTHREAD(opt):
    global BuildOptions
    BuildOptions = opt

def PrepareBuilding(env):
    global Env
    Env = env
    env['RTOS']='askar'
    if(os.getenv('RTOS')=='rtthread'):
        env['RTOS'] = 'rtthread'
    print('RTOS is %s'%(env['RTOS']))
    # add comstr option
    AddOption('--verbose',
            dest='verbose',
            action='store_true',
            default=False,
            help='print verbose information during build')

    if(not GetOption('verbose')):
    # override the default verbose command string
        env.Replace(
          ARCOMSTR = 'AR $SOURCE',
          ASCOMSTR = 'AS $SOURCE',
          ASPPCOMSTR = 'AS $SOURCE',
          CCCOMSTR = 'CC $SOURCE',
          CXXCOMSTR = 'CXX $SOURCE',
          LINKCOMSTR = 'LINK $TARGET'
        )

def GetCurrentDir():
    conscript = File('SConscript')
    fn = conscript.rfile()
    path = os.path.dirname(fn.abspath)
    return path

def MKDir(p):
    ap = os.path.abspath(p)
    pap = os.path.abspath('%s/..'%(p))
    if(not os.path.exists(pap)):
        MKDir(pap)
    if(not os.path.exists(ap)):
        os.mkdir(ap)

def RMDir(p):
    shutil.rmtree(p)

def MKFile(p,c='',m='wb'):
    f = open(p,m)
    f.write('c')
    f.close()

def MKSymlink(src,dst):
    asrc = os.path.abspath(src)
    adst = os.path.abspath(dst)
    if(not os.path.exists(dst)):
        os.symlink(asrc,adst)

def SrcRemove(src, remove):
    if not src:
        return

    for item in src:
        if type(item) == type('str'):
            if os.path.basename(item) in remove:
                src.remove(item)
        else:
            if os.path.basename(item.rstr()) in remove:
                src.remove(item)

def RunCommand(cmd):
    print(' >> RunCommand "%s"'%(cmd))
    if(0 != os.system(cmd)):
        raise Exception('FAIL of RunCommand "%s"'%(cmd))

def DefineGroup(name, src, depend, **parameters):
    global Env
    if not GetDepend(depend):
        return []
    if parameters.has_key('CCFLAGS'):
        Env.AppendUnique(CCFLAGS = parameters['CCFLAGS'])
    if parameters.has_key('CPPPATH'):
        Env.AppendUnique(CPPPATH = parameters['CPPPATH'])
    if parameters.has_key('CPPDEFINES'):
        Env.AppendUnique(CPPDEFINES = parameters['CPPDEFINES'])
    if parameters.has_key('LINKFLAGS'):
        Env.AppendUnique(LINKFLAGS = parameters['LINKFLAGS'])
    objs = []    
    for obj in src:
        objs.append(Glob(str(obj)))
    return objs

def AddDepend(option):
    BuildOptions[option] = 1

def GetDepend(depend):
    building = True
    if type(depend) == type('str'):
        if not BuildOptions.has_key(depend) or BuildOptions[depend] == 0:
            building = False
        elif BuildOptions[depend] != '':
            return BuildOptions[depend]

        return building

    # for list type depend
    for item in depend:
        if item != '':
            if not BuildOptions.has_key(item) or BuildOptions[item] == 0:
                building = False

    return building

def GetConfigValue(name):
    assert type(name) == str, 'GetConfigValue: only string parameter is valid'
    try:
        return BuildOptions[name]
    except:
        return ''

