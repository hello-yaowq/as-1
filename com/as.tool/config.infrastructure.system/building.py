import os
import sys
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
            _e['PATH'] = Env['EXTRAPATH']+';'+_e['PATH']
        except KeyError:
            pass

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
    GetConfig('.config',env)
    env['python3'] = 'python3'
    env['python2'] = 'python2'
    env['python'] = 'python'
    env['pkgconfig'] = 'pkg-config'
    env['msys2'] = False
    if(sys.platform=='msys'):
        # force os name to 'nt'
        os.name = 'nt'
        raise Exception('Native scons of msys is not supported yet!')
    if((os.name == 'nt') and (env['CONFIGS'] != None)):
        env['python3'] = env['CONFIGS']['PYTHON3_PATH'] + '/python'
        env['python2'] = env['CONFIGS']['PYTHON2_PATH'] + '/python'
        env['python']  = env['CONFIGS']['PYTHON2_PATH'] + '/python'
        if('MINGW_GCC_PATH' in env['CONFIGS']):
            env['CC'] = env['CONFIGS']['MINGW_GCC_PATH'] + '/gcc'
            env['pkgconfig'] = env['CONFIGS']['MINGW_GCC_PATH'] + '/pkg-config'
        elif('MSYS2_GCC_PATH' in env['CONFIGS']):
            env['msys2'] = True
            mpath = env['CONFIGS']['MSYS2_GCC_PATH']
            env['CC'] = mpath + '/gcc'
            env['pkgconfig'] = mpath + '/pkg-config'
            env['EXTRAPATH'] = '%s;%s'%(mpath, os.path.abspath(mpath+'/../usr/bin'))
    elif(os.name == 'nt'):
        env['python3'] = 'c:/Anaconda3/python.exe'
        env['python2'] = 'c:/Python27/python.exe'
        env['python'] =  'c:/Python27/python.exe'
        uname = RunSysCmd('uname')
        if(uname.startswith('MSYS_NT')):
            print('build on %s, default assume 64 bit machine'%(uname.strip()))
            env['msys2'] = True
            env['pkgconfig'] = 'c:/msys64/mingw64/bin/pkg-config'
            env['CC'] = 'c:/msys64/mingw64/bin/gcc'
            env['LINK'] = 'c:/msys64/mingw64/bin/gcc'
            env['EXTRAPATH'] = 'c:/msys64/mingw64/bin;c:/msys64/usr/bin'
    if(os.name == 'nt'):
        win32_spawn = Win32Spawn()
        env['SPAWN'] = win32_spawn.spawn
    if(0 != os.system('%s --version'%(env['python3']))):
        raise Exception('no python3 installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['python2']))):
        raise Exception('no python2 installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['python']))):
        raise Exception('no python installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['pkgconfig']))):
        raise Exception('no pkg-config installed, fix the path maybe!')
    if(0 != os.system('%s --version'%(env['CC']))):
        raise Exception('no C Compiler installed, fix the path maybe!')
    env['CXX'] = env['CC']
    # add comstr option
    AddOption('--verbose',
            dest='verbose',
            action='store_true',
            default=False,
            help='print verbose information during build')
    AddOption('--force',
            dest='force',
            action='store_true',
            default=False,
            help='force rebuild of all')
    AddOption('--menuconfig', 
            dest = 'menuconfig',
            action = 'store_true',
            default = False,
            help = 'make menuconfig for Automotive Software AS')

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

    if(GetOption('menuconfig')):
        menuconfig(env)

def mk_rtconfig(filename):
    try:
        config = file(filename)
    except:
        print('open .config failed')
        return
    rtcfg = os.path.abspath('../../com/as.infrastructure/system/kernel/rtthread/menuconfig/rtconfig.h')
    if(not os.path.exists(rtcfg)):
        print('%s is not exits!'%(rtcfg))
    rtconfig = file(rtcfg, 'w')
    rtconfig.write('#ifndef RT_CONFIG_H__\n')
    rtconfig.write('#define RT_CONFIG_H__\n\n')

    empty_line = 1

    for line in config:
        line = line.lstrip(' ').replace('\n', '').replace('\r', '')

        if(len(line) == 0): continue

        if(line[0] == '#'):
            if len(line) == 1:
                if empty_line:
                    continue

                rtconfig.write('\n')
                empty_line = 1
                continue

            comment_line = line[1:]
            if(line.startswith('# CONFIG_')): line = ' ' + line[9:]
            else: line = line[1:]

            rtconfig.write('/*%s */\n' % line)
            empty_line = 0
        else:
            empty_line = 0
            setting = line.split('=')
            if(len(setting) >= 2):
                if(setting[0].startswith('CONFIG_')):
                    setting[0] = setting[0][7:]

                # remove CONFIG_PKG_XX_PATH or CONFIG_PKG_XX_VER
                if(type(setting[0]) == type('a') and (setting[0].endswith('_PATH') or setting[0].endswith('_VER'))):
                    continue

                if(setting[1] == 'y'):
                    rtconfig.write('#define %s\n' % setting[0])
                else:
                    rtconfig.write('#define %s %s\n' % (setting[0], setting[1]))

    rtconfig.write('\n')
    rtconfig.write('#endif\n')
    rtconfig.close()
    print('update %s done!'%(rtcfg))

def GetConfig(cfg,env):
    import re
    if(not os.path.exists(cfg)):
        # None to use default confiuration
        env['MODULES'] = None
        env['CONFIGS'] = None
        print('WARNING: no menuconfig file(".config") found, will use default configuration!')
        return
    env['MODULES'] = []
    env['CONFIGS'] = {}
    reOne = re.compile(r'([^\s]+)\s*=\s*([^\s]+)')
    fp = open(cfg)
    for el in fp.readlines():
        if(reOne.search(el)):
            name,value = reOne.search(el).groups()
            name = name.replace('CONFIG_','')
            if(value=='y'):
                env['MODULES'].append(name)
            else:
                if(value[0]=='"'): value = value[1:]
                if(value[-1]=='"'): value = value[:-1]
                env['CONFIGS'][name]=value
    fp.close()
    print('Modules:',env['MODULES'])
    print('Configs:',env['CONFIGS'])

def menuconfig(env):
    import time
    kconfig = '%s/com/as.tool/kconfig-frontends/kconfig-mconf'%(env['ASROOT'])
    cmd = ''
    if(os.name == 'nt'):
        kconfig += '.exe'
        cmd += 'set BOARD=%s && set ASROOT=%s && start cmd /C '%(env['BOARD'],env['ASROOT'])
    else:
        cmd += 'export BOARD=%s && export ASROOT=%s && '%(env['BOARD'],env['ASROOT'])
    if(not os.path.exists(kconfig)):
        RunCommand('cd %s/com/as.tool/kconfig-frontends && make'%(env['ASROOT']))
    if(os.path.exists(kconfig)):
        assert(os.path.exists('Kconfig'))
        cmd += kconfig + ' Kconfig'

        fn = '.config'
        if(os.path.isfile(fn)):
            mtime = os.path.getmtime(fn)
        else:
            mtime = -1
        RunCommand(cmd)
        print('press Ctrl+C to exit!')
        if(os.name == 'nt'): 
            while(True): 
                time.sleep(1)
                if(os.path.isfile(fn)):
                    mtime2 = os.path.getmtime(fn)
                else:
                    mtime2 = -1
                if(mtime != mtime2):
                    break
        if(os.path.isfile(fn)):
            mtime2 = os.path.getmtime(fn)
        else:
            mtime2 = -1
        if(mtime != mtime2):
            GetConfig(fn,env)
            if('RTTHREAD' in env['MODULES']):
                mk_rtconfig(fn)
        exit(0)
    else:
        raise Exception("can't find out %s"%(kconfig))

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
    f.write(c)
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
            fp.write('%s\n'%(c.strip()))
        fp.close()
        cmd = '.scons.bat'
    if(0 != os.system(cmd)):
        raise Exception('FAIL of RunCommand "%s"'%(cmd))

def RunSysCmd(cmd):
    import subprocess
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    (output, err) = p.communicate()
    p_status = p.wait()
    return output

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

def GetELFEnv():
    global Env
    cwd = os.path.abspath(os.path.curdir)
    if(Env.GetOption('clean')):
        RMFile('aself.lds')
    if(not os.path.exists('aself.lds')):
        MKFile('aself.lds',
               '''SECTIONS
{
  . = 0;
  .text : { *(.text*) }
  . = ALIGN(4);
  .data : { *(.data*) }
  . = ALIGN(4);
  .bss : { *(.bss*) }
  . = ALIGN(4);
  .rodata : { *(.rodata*) }
  . = ALIGN(4);
  .rel.plt : { *(.rel.*) }
}\n''', 'w')
    env = Environment(CC=Env['CC'],
                      LINK=Env['CC'],
                      CPPPATH=Env['CPPPATH'],
                      CCFLAGS=Env['CCFLAGS']+['-fPIC'],
                      LINKFLAGS=['-e','main','-fPIC','-T','%s/aself.lds'%(cwd)],
                      SHLINKFLAGS=['-fPIC','-shared','-s','-nostdlib',
                                   '-T','%s/aself.lds'%(cwd)])
    for flg in ['-ffunction-sections','-fdata-sections','-g']:
        if(flg in env['CCFLAGS']):
            env['CCFLAGS'].remove(flg)
    if(not GetOption('verbose')):
    # override the default verbose command string
        env.Replace(
          ARCOMSTR = 'AR $SOURCE',
          SHCCCOMSTR = 'SHCC $SOURCE',
          SHCXXCOMSTR = 'SHCXX $SOURCE',
          SHLINKCOMSTR = 'SHLINK $TARGET'
        )
    if(os.name == 'nt'):
        env['SHLINKCOM'] = '$SHLINK $SHLINKFLAGS $SOURCES -o $TARGET'
    return env
