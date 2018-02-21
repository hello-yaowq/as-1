import os
import sys
import shutil
import string
import re
from SCons.Script import *
import xcc
import argen

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
    env['POSTACTION'] = ''
    if(sys.platform=='msys'):
        # force os name to 'nt'
        os.name = 'nt'
        raise Exception('Native scons of msys is not supported yet!')
    if((os.name == 'nt') and (env['CONFIGS'] != None)):
        env['python3'] = env['CONFIGS']['PYTHON3_PATH'] + '/python'
        env['python2'] = env['CONFIGS']['PYTHON2_PATH'] + '/python'
        env['python']  = env['CONFIGS']['PYTHON2_PATH'] + '/python'
        if('PLATFORM_MINGW' in env['MODULES']):
            env['CC'] = env['CONFIGS']['MINGW_GCC_PATH'] + '/gcc'
            env['pkgconfig'] = env['CONFIGS']['MINGW_GCC_PATH'] + '/pkg-config'
        elif('PLATFORM_MSYS2' in env['MODULES']):
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
            cfgdir = 'build/%s/config'%(env['BOARD'])
            MKDir(cfgdir)
            xcc.XCC(cfgdir,env)
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
        print('removing %s'%(os.path.abspath(p)))
        os.remove(os.path.abspath(p))

def MKObject(src, tgt, cmd):
    if(Env.GetOption('clean')):
        RMFile(tgt)
        return
    mtime = os.path.getmtime(src)
    if(os.path.isfile(tgt)):
        mtime2 = os.path.getmtime(tgt)
    else:
        mtime2 = -1
    if(mtime2 < mtime):
        RunCommand(cmd)

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
                src.remove(str(item))
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

def GetELFEnv(so=True):
    global Env
    mmap = '%s/release/%s/build/%s/%s.map'%(Env['ASROOT'], Env['RELEASE'],
                Env['BOARD'],Env['BOARD'])
    cstr = ''
    if(os.path.exists(mmap) and (so==False)):
        # for executale files, should know all basic API address
        reSym = re.compile(r'^\s+(0x\w+)\s+([a-zA-Z_]\w+)$')
        ignore = ['main']
        fp = open(mmap)
        for el in fp.readlines():
            if(reSym.search(el)):
                grp = reSym.search(el).groups()
                if(grp[1] in ignore):continue
                cstr += '%s = %s;\n'%(grp[1], grp[0])
        fp.close()
    elif(so == False):
        return None
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
}\n\n%s\n'''%(cstr), 'w')
    env = Environment(CC=Env['CC'],
                      LINK=Env['CC'],
                      CPPPATH=Env['CPPPATH'],
                      CCFLAGS=Env['CCFLAGS']+['-fPIC'],
                      LINKFLAGS=['-e','main','-fPIC','-s','-nostdlib','-T','%s/aself.lds'%(cwd)],
                      SHLINKFLAGS=['-fPIC','-shared','-s','-nostdlib',
                                   '-T','%s/aself.lds'%(cwd)])
    for flg in ['-ffunction-sections','-fdata-sections','-g']:
        if(flg in env['CCFLAGS']):
            env['CCFLAGS'].remove(flg)
    if(not GetOption('verbose')):
    # override the default verbose command string
        env.Replace(
          ARCOMSTR = 'AR $SOURCE',
          CCCOMSTR = 'ELFCC $SOURCE',
          CXXCOMSTR = 'ELFCXX $SOURCE',
          LINKCOMSTR = 'ELFLINK $TARGET',
          SHCCCOMSTR = 'SHCC $SOURCE',
          SHCXXCOMSTR = 'SHCXX $SOURCE',
          SHLINKCOMSTR = 'SHLINK $TARGET'
        )
    if(os.name == 'nt'):
        env['SHLINKCOM'] = '$SHLINK $SHLINKFLAGS $SOURCES -o $TARGET'
    return env

class Qemu():
    def __init__(self, qemu=None):
        arch_map = {'x86':'i386','cortex-m':'arm'}
        ASROOT = Env['ASROOT']
        ARCH = Env['ARCH']
        self.arch = Env['arch']
        self.params = ''
        self.params = '-serial tcp:127.0.0.1:1103,server'
        if('gdb' in COMMAND_LINE_TARGETS):
            self.params += ' -gdb tcp::1234 -S'
        if(self.arch in arch_map.keys()):
            self.arch = arch_map[self.arch]
        if(qemu is None):
            self.isAsQemu = True
            self.qemu = self.LocateASQemu()
            self.CreateDiskImg('%s/release/%s/asblk0.img'%(ASROOT,Env['RELEASE']), 32*1024*1024, 'vfat')
            self.CreateDiskImg('%s/release/%s/asblk1.img'%(ASROOT,Env['RELEASE']), 32*1024*1024, 'ext4')
        else:
            self.isAsQemu = False
            self.qemu = qemu

    def LocateASQemu(self):
        ASROOT = Env['ASROOT']
        if(os.name == 'nt'):
            candrvsrc = '%s/com/as.tool/lua/can/socketwin_can_driver.c'%(ASROOT)
            candrvtgt = '%s/com/as.tool/lua/script/socketwin_can_driver.exe'%(ASROOT)
            cmd = '%s -D__SOCKET_WIN_CAN_DRIVER__ %s -lwsock32 -o %s'%(Env['CC'], candrvsrc, candrvtgt)
            MKObject(candrvsrc, candrvtgt, cmd)
            # try default install location of qemu
            try:
                qemu = '%s/qemu-system-%s'%(Env['CONFIGS']['MSYS2_GCC_PATH'],self.arch)
            except:
                qemu = '%s/qemu-system-%s'%(Env['CC'][:-3],self.arch)
            if(not os.path.exists(qemu+'.exe')):
                qemu = '%s/com/as.tool/qemu/src/build-x86_64-w64-mingw32/%s-softmmu/qemu-system-%s'%(ASROOT, self.arch, self.arch)
        else:
            qemu = '%s/release/download/qemu/%s-softmmu/qemu-system-%s'%(ASROOT, self.arch, self.arch)
        if(os.name == 'nt'):
            qemu += '.exe'
        if(not os.path.exists(qemu)):
            print('%s is not exits, try build it out locally!'%(qemu))
            self.BuildASQemu()
        self.params += ' -device pci-ascan -device pci-asnet -device pci-asblk'
        return qemu

    def Run(self, params, where=None):
        ASROOT = Env['ASROOT']
        build = '%s/release/%s'%(ASROOT, Env['RELEASE'])
        if(where is None):
            where = build
        python = Env['python3']
        if(os.name == 'nt'):
            python = 'start ' + python
        if('asone' in COMMAND_LINE_TARGETS):
            RunCommand('cd %s/com/as.tool/as.one.py && %s main.py'%(ASROOT,python))
        if(os.name == 'nt'):
            if(self.isAsQemu):
                RunCommand('start %s/com/as.tool/lua/script/socketwin_can_driver.exe 0'%(ASROOT))
                RunCommand('start %s/com/as.tool/lua/script/socketwin_can_driver.exe 1'%(ASROOT))
            RunCommand('cd %s && start cmd /C %s %s %s'%(where, self.qemu, params, self.params))
            RunCommand('sleep 2 && telnet 127.0.0.1 1103')
        else:
            fp = open('%s/telnet.sh'%(build),'w')
            fp.write('sleep 0.5\ntelnet 127.0.0.1 1103\n')
            fp.close()
            fp = open('%s/qemu.sh'%(build),'w')
            fp.write('%s %s %s & sh %s/telnet.sh\nsleep 60\n'%(self.qemu,params,self.params,build))
            fp.close()
            RunCommand('sudo pgrep qemu-system-%s | xargs -i kill -9 {}'%(self.arch))
            RunCommand('cd %s && chmod +x %s/*.sh && sudo gnome-terminal -x %s/qemu.sh'%(build,build,build))
        exit(0)

    def CreateDiskImg(self, file, size, type='raw'):
        ASROOT = Env['ASROOT']
        if(os.path.exists(file)):
            print('DiskImg "%s" already exist!'%(file))
            return
        print('Create a New DiskImg "%s"!'%(file))
        if(os.name == 'nt'):
            # try default install location of qemu
            try:
                qemuimg = '%s/qemu-img'%(Env['CONFIGS']['MSYS2_GCC_PATH'])
            except:
                qemuimg = '%s/qemu-img'%(Env['CC'][:-3])
            if(not os.path.exists(qemuimg+'.exe')):
                qemuimg = '%s/com/as.tool/qemu/src/build-x86_64-w64-mingw32/qemu-img'%(ASROOT)
        else:
            qemuimg = '%s/release/download/qemu/qemu-img'%(ASROOT)
            if(not os.path.exists(qemuimg)):
                qemuimg = 'qemu-img'

        RunCommand('%s create -f raw %s %s'%(qemuimg, file, size))

        if(type.startswith('ext')):
            if(os.name == 'nt'):
                lwext4mkfs = '%s/release/download/lwext4/build_generic/fs_test/lwext4-mkfs.exe'%(ASROOT)
                RunCommand('%s -i %s -b 4096 -e %s'%(lwext4mkfs,file,type[3]))
            else:
                RunCommand('sudo mkfs.%s -b 4096 %s'%(type,file))
        elif(type.startswith('vfat')):
            if(os.name == 'nt'):
                pass # TODO
            else:
                RunCommand('sudo mkfs.fat %s'%(file))

    def BuildASQemu(self):
        ASROOT = Env['ASROOT']
        if(os.name == 'nt'):
            mpath = os.path.abspath(Env['CONFIGS']['MSYS2_GCC_PATH']+"/../..")
            RunCommand('%s/msys2_shell.cmd -mingw64 -where %s/com/as.tool/qemu'%(mpath,ASROOT))
            print('please mannuly invoke below comand in the poped up msys2 window:')
            print('\tMINGW_INSTALLS=mingw64 makepkg-mingw -sLf')
            print('\tpacman -U mingw-w64-x86_64-qemu-2.10.0-1-any.pkg.tar.xz')
            print('and then do "scons run" again')
            exit(-1)
        else:
            RunCommand('cd %s/release/ascore && make asqemu'%(ASROOT))

def SelectCompilerARMICC(iarPath=None):
    import glob
    iar = iarPath
    if(iar is None):
        # try search path of IAR
        for disk in ['C:/','D:/','E:/','F:/']:
            for prg in ['Program Files (x86)','Program Files','ProgramData']:
                iar = os.path.join(disk, prg, 'IAR Systems')
                if(os.path.exists(iar)):
                    for bench in glob.glob(os.path.join(iar,'*')):
                        iar = os.path.join(bench,'arm')
                        if(os.path.exists(os.path.join(iar,'bin','iccarm.exe'))):
                            print('IAR:: %s'%(iar))
                            iarPath = iar
    if(iarPath is not None):
        print('IAR:: %s <== using this one'%(iarPath))
        Env['CC']=os.path.join(iarPath,'bin','iccarm.exe')
        Env['AS']=os.path.join(iarPath,'bin','iasmarm.exe')
        Env['LINK']=os.path.join(iarPath,'bin','ilinkarm.exe')
        Env['AR']=os.path.join(iarPath,'bin','iarchive.exe')

    return iarPath

def SelectCompilerArmNoneEabi():
    global Env
    ASROOT = Env['ASROOT']
    Env['CC']='arm-none-eabi-gcc -std=gnu99'
    Env['CXX']='arm-none-eabi-g++'
    Env['AS']='arm-none-eabi-as'
    Env['LINK']='arm-none-eabi-ld'
    Env['S19'] = 'arm-none-eabi-objcopy -O srec --srec-forceS3 --srec-len 32'
    if(os.name == 'nt'):
        gccarm = 'gcc-arm-none-eabi-5_4-2016q3-20160926-win32'
        gccsrc= 'https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/%s.zip'%(gccarm)
        cpl = '%s/release/download/%s'%(ASROOT,gccarm)
        if(not os.path.exists(cpl)):
            RunCommand('cd %s/release/download && wget %s && mkdir -p %s && cd %s && unzip ../%s.zip'%(ASROOT,gccsrc,gccarm,gccarm,gccarm))
        Env.Append(LIBPATH=['%s/lib/gcc/arm-none-eabi/5.4.1'%(cpl)])
        Env.Append(LIBPATH=['%s/arm-none-eabi/lib'%(cpl)])
        Env['CC']='%s/bin/arm-none-eabi-gcc -std=gnu99'%(cpl)
        Env['CXX']='%s/bin/arm-none-eabi-g++'%(cpl)
        Env['AS']='%s/bin/arm-none-eabi-gcc -c'%(cpl)
        Env['LINK']='%s/bin/arm-none-eabi-ld'%(cpl)
        Env['S19'] = '%s/bin/%s'%(cpl,Env['S19'])
    else:
        # FIXME to the right path
        libgcc = '/usr/lib/gcc/arm-none-eabi/4.8.2'
        assert(os.path.exists(libgcc))
        Env.Append(LIBPATH=[libgcc,'/usr/lib/arm-none-eabi/newlib'])

def BuildOFS(ofs):
    for of in ofs:
        src = str(of)
        tgt = src[:-3]+'.h'
        cflags = ''
        for p in Env['CPPPATH']:
            cflags += ' -I%s'%(p)
        for d in Env['CPPDEFINES']:
            cflags += ' -D%s'%(d)
        cmd = 'cp %s .tmp.c && %s -S .tmp.c -o .tmp.S %s'%(src, Env['CC'], cflags)
        cmd += ' && sed -n "/#define/p" .tmp.S > %s'%(tgt)
        MKObject(src, tgt, cmd)
