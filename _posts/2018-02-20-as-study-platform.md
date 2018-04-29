---
layout: post
title: AS stduy platform -- qemu
category: AUTOSAR
comments: true
---

This article will introduce that how to use the qemu to stduy AS on windows, for that on linux, it's almost the same way.

# 1. AS supported machine list

The virtual qemu machine versatilepb and x86 are the best supported by AS, for the others, it's legeacy work that now has no any updates, it maybe even not works properly.

## 1.1 arm926 versatilepb and x86 i386

These 2 are the best supported qemu machines by AS, as that both of them have qemu PCI bus, so it's very easy to develop some special device base on the qemu PCI bus, the below are 3 devices developed by me but with a limitation that doesn't support ISR:

* [asnet](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/asnet.c) : simulation of the ethernet
* [ascan](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/ascan.c) : simulation of the CAN(CANFD)
* [asblk](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/asblk.c) : simulation of the storage block device

## 1.2 [arm926 3sc2440a](https://github.com/parai/as/blob/master/com/as.application/board.s3c2440a/qemu)

This is only supported on windows as the qemu machine original precompiled by rtthread maybe, you can use it to study [trampoline](https://github.com/TrampolineRTOS/trampoline) only, the ASKAR RTOS doesn't run on this machine as I am lazy that I don't want to do the porting work as I think versatilepb and x86 are good enough.

## 1.3 cortex-m3 lm3s6965evb

I didn't get the dispatch(asm "svc 0") works now as it may hardfaut, so it can only run cortex-m3 code without ISR and task context switch. 

# 2. Set up environment on windows

## 2.1 install [Anaconda3 64bit](https://www.anaconda.com/download/) as C:\Anaconda3

## 2.2 install [msys2 x86_64](http://www.msys2.org/) as C:\msys64

## 2.3 then run below commands in cmd

```sh
set PATH=C:\Anaconda3;C:\Anaconda3\Scripts;C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%
pacman -Sy
pacman -S unzip wget git mingw-w64-x86_64-gcc mingw-w64-x86_64-glib2 mingw-w64-x86_64-gtk3
pacman -S ncurses-devel gperf curl
pacman -Syuu
wget https://pypi.python.org/packages/1f/3b/ee6f354bcb1e28a7cd735be98f39ecf80554948284b41e9f7965951befa6/pyserial-3.2.1.tar.gz#md5=7142a421c8b35d2dac6c47c254db023d
tar xf pyserial-3.2.1.tar.gz
cd pyserial-3.2.1
python setup.py install
conda install scons
```

# 3. How to build ascore and run

This is very easy!

## 3.1 launch the [/as/Console.bat](https://github.com/parai/as/blob/master/Console.bat) by double click

* Note: for the first time build, it will be very slow as it need to dynamic download some packages and tools to directory "/as/release/download"

## 3.2 choose a board want to be used

* Note: x86 can only be built out by Ubunut.

![scons-set-board.png](/as/images/rewoa/scons-set-board.png)

## 3.3 do menuconfig by command "```scons --menuconfig```"

Generally it's better to remove the previous config by command "rm .config" when switch the BOARD, and it's okay to leave all conifig as default.

![scons-menuconfig.png](/as/images/rewoa/scons-menuconfig.png)

## 3.3 do AUTOSAR config by command "scons studio"

* Note: The OS panel is an additional OS configuration to the *.xml OS configuration, you can check the xml files under directory "/as/release/ascore/build/$BOARD/config" for how to add a Task/Alarm/Counter/Resource etc by new a xml or use the OS panel to do OS configuration.

Generally you can leave it as default.

![scons-studio](/as/images/rewoa/scons-studio.png)

## 3.4 do build by command "scons" and do clean by command "scons -c"

## 3.5 after build sucessfully, we can run it now by command "scons run".

For the first time, it will pop up a msys2 windows which its path is "/as/com/as.tool/qemu", in which the below 2 command should be executed to compile the qemu out and then install it.

```sh
MINGW_INSTALLS=mingw64 makepkg-mingw -sLf
pacman -U mingw-w64-x86_64-qemu-2.10.0-1-any.pkg.tar.xz
```

![scons-runqemubuild](/as/images/rewoa/scons-runqemubuild.png)

The below picture gives a demo that how to use FTP to push a file to ascore VFS filesystem(somehow a test of LWIP on OSEK OS) and how to use the ascore libelf to execute the "main" function of a DLL.

![scons-run](/as/images/rewoa/scons-run.png)

## 3.6 debug by eclipse

### 3.6.1 firstly, run command "scons run gdb" to launch the qemu machine with gdb server, which waiting the connection from eclipse plugin qemu-dbg

### 3.6.2 make sure the eclipse has installed the plugin "GNU MCU C/C++ QEMU Debugging", and then follow below steps to start the debug.

New a qemu debug configuration, and as showed by below picture, click "browse" to select the build out target(*.exe) under directory "/as/release/ascore".

![scons-rungdb-eclipse-debugcfg0](/as/images/rewoa/scons-rungdb-eclipse-debugcfg0.png)

And then follow below pictue, "browse" to select the $BOARD related gdb. For ARM, select the "/as/release/download/gcc-arm-none-eabi-5_4-2016q3-20160926-win32/bin/arm-none-eabi-gdb.exe". Fox x86, just select the "/usr/bin/gdb".

![scons-rungdb-eclipse-debugcfg1](/as/images/rewoa/scons-rungdb-eclipse-debugcfg1.png)

Then click the button "Debug" to start debug.

![scons-rungdb-eclipse-debug.png](/as/images/rewoa/scons-rungdb-eclipse-debug.png)

# 4. as.one.py - a powerful pyhton tool

The as.one.py is a very powerful python tool has integrated a lot features, such as BOOTLOADER, Diagnostic Communication Control(UDS iso14229) on CAN or DoIP(ethernet) and XCP(Universal Calibration Protocol).

For the purpose to use it, it must compiles the aslua firstly to build out the necessary library AS.pyd(this is generally for CAN).

Run command "make clean && make aslua" in the panel of aslua of the Console, but as the issue of msys2, you will encounter below error maybe.

```sh
D:\repository\as\release\ascore>make aslua
....
D:/repository/as/release/aslua/out/libaws.a(random_seed.o): In function `GetCurrentFiber':
C:/msys64/mingw64/x86_64-w64-mingw32/include/winnt.h:8604: multiple definition of `GetCurrentFiber'
D:/repository/as/release/aslua/out/libaws.a(asocket.o):C:/msys64/mingw64/x86_64-w64-mingw32/include/winnt.h:8604: first defined
here
D:/repository/as/release/aslua/out/libaws.a(random_seed.o): In function `GetFiberData':
C:/msys64/mingw64/x86_64-w64-mingw32/include/winnt.h:8605: multiple definition of `GetFiberData'
D:/repository/as/release/aslua/out/libaws.a(asocket.o):C:/msys64/mingw64/x86_64-w64-mingw32/include/winnt.h:8605: first defined
here
collect2.exe: error: ld returned 1 exit status
make[1]: *** [/d/repository/as//com/as.tool/lua/pyas/Makefile:46: AS.pyd] Error 1
make[1]: Leaving directory '/d/repository/as/release/aslua/out'
make: *** [Makefile:409: 82] Error 2
```

This issue is very easy to be fixed, for example for "multiple definition of `GetFiberData'", just open file "C:/msys64/mingw64/x86_64-w64-mingw32/include/winnt.h", goto line 8605, do the modification as below picture shows:

![msys2-aslua-build-issue-fix](/as/images/rewoa/msys2-aslua-build-issue-fix.png)

Adter fix all those kind of issues, the aslua can be built sucesfully by "make clean && make aslua".

Then in the panel of "ascore", run the command "scons run asone" to run with the tool as.one.py also started or just in the panel of "as.one.py", run command "python main.py".

That's all! Have Fun with AS.
