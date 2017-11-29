import os
import shutil
import string
from SCons.Script import *

Env = None

BuildOptions = {}

class Win32Spawn:
    def spawn(self, sh, escape, cmd, args, env):
        # deal with the cmd build-in commands which cannot be used in
        # subprocess.Popen
        if cmd == 'del':
            for f in args[1:]:
                try:
                    os.remove(f)
                except Exception as e:
                    print('Error removing file: %s'%(e))
                    return -1
            return 0

        import subprocess

        newargs = string.join(args[1:], ' ')
        cmdline = cmd + " " + newargs

        # Make sure the env is constructed by strings
        _e = dict([(k, str(v)) for k, v in env.items()])

        # Windows(tm) CreateProcess does not use the env passed to it to find
        # the executables. So we have to modify our own PATH to make Popen
        # work.
        old_path = os.environ['PATH']
        os.environ['PATH'] = _e['PATH']

        try:
            proc = subprocess.Popen(cmdline, env=_e, shell=False)
        except Exception as e:
            print('Error in calling:\n%s'%(cmdline))
            print('Exception: %s: %s'%(e, os.strerror(e.errno)))
            return e.errno
        finally:
            os.environ['PATH'] = old_path

        return proc.wait()

def PrepareRTTHREAD(opt):
    global BuildOptions
    BuildOptions = opt

def PrepareBuilding(env):
    global Env
    Env = env
    env['python3'] = 'python3'
    env['python2'] = 'python2'
    env['python'] = 'python'
    if(os.name == 'nt'):
        win32_spawn = Win32Spawn()
        win32_spawn.env = env
        env['SPAWN'] = win32_spawn.spawn
        if(os.path.exists('c:/Python34')):
            env['python3'] = 'c:/Python34/python'
        else:
            env['python3'] = 'c:/Python36/python'
        env['python2'] = 'c:/Python27/python'
        env['python'] =  'c:/Python27/python'
    if(0 != os.system('%s --version'%(env['python3']))):
        raise Exception('no python3 installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['python2']))):
        raise Exception('no python2 installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['python']))):
        raise Exception('no python installed, fix the path maybe!')
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
    if(os.path.exists(p)):
        shutil.rmtree(p)

def RMFile(p):
    if(os.path.exists(p)):
        os.remove(p)

def MKFile(p,c='',m='wb'):
    f = open(p,m)
    f.write('c')
    f.close()

def MKSymlink(src,dst):
    asrc = os.path.abspath(src)
    adst = os.path.abspath(dst)
    if(not os.path.exists(dst)):
        if(os.name=='nt'):
            if(os.path.isdir(asrc)):
                RunCommand('mklink /D %s %s'%(adst,asrc))
            else:
                RunCommand('mklink %s %s'%(adst,asrc))
        else:
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
    if(os.name=='nt'):
        cmds = cmd.split('&&')
        fp = open('.scons.bat','w')
        fp.write('@echo off\n')
        for c in cmds:
            fp.write('%s\n'%(c))
        fp.close()
        cmd = '.scons.bat'
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

