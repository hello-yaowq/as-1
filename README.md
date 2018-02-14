# as
automotive software and its tool-chain

[![Build Status](https://travis-ci.org/parai/as.svg?branch=master)](https://travis-ci.org/parai/as)

## why this repository
Because I am not powerful so I decided to develop small but smart part of automotive software based on open source, and create a general AUTOSAR & Automotive Software study environment.

As below picture shows, it was an virtual cluster based on AUTOSAR and can be simulated on Windows and Linux.

![ascore posix vic on AUTOSAR](https://github.com/parai/as/raw/gh-pages/images/ascore_posix_vic.gif)

## Key Point

### 1. Integrated ArcCore AUTOSAR 3.1 BSW
* Communication: Can(CanFD) CanIf PduR Com SoAd
* Diagnostic: CanTp Dcm DoIP Dem
* System: EcuM SchM
* Memory: Fls Fee Eep Ea NvM MemIf

### 2. OS 
* [toppers_osek](https://www.toppers.jp/osek-os.html)
* [FreeOSEK](http://opensek.sourceforge.net/)
* [FreeRTOS](http://www.freertos.org/)
* [ucos ii](https://www.micrium.com/)
* [toppers-atk2](https://www.toppers.jp/atk2.html)
* [small](https://github.com/parai/as/tree/master/com/as.infrastructure/system/kernel/small) : this OS is developed by me, OSEK BCC1 conformed
* [contiki](http://contiki-os.org/)
* [trampoline](https://github.com/TrampolineRTOS/trampoline)
* [rtthread](https://github.com/RT-Thread/rt-thread)
* [ASKAR](https://github.com/parai/as/tree/master/com/as.infrastructure/system/kernel/askar) : [Automotive oSeK AutosaR](http://parai.github.io/as/autosar/2017/10/27/ASKAR-architecture.html) designed by me, [OSEK/VDX OS223](http://trampolinebin.rts-software.org/os223.pdf) confirmed.

## 3. BSW configuration tool
> This GUI tool is the one from [OpenSAR](https://github.com/parai/OpenSAR.git) which is also developed by me, the GUI can be automatically created from xml.

![as configure tool](http://parai.github.io/as/images/config.infrastructure.system.png)

## 4. AUTOSAR & LINUX communication
* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)

## 5. Lua & Python Integrated
* support CAN device by lascanlib or pyas: CANcaseXL, [PeakCAN](http://www.peak-system.com/PCAN-USB.199.0.html?L=1), [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN), [ZLG USBCAN](http://www.zlg.cn/can/can/product/id/22.html),also provide a kind of python library for CAN access.
* support miscellaneous device by lasdevlib with the smae API interface "open/read/write/ioctl/close": rs232
* a powerful python as.one.py tool: Dcm Xcp Bootloader and others.

![as.one.py bootloader](http://parai.github.io/as/images/python3-asone-tool.png)

## 6. aslinux simulation environment on QEMU
* Now use one [makefile](https://github.com/parai/as/blob/master/release/aslinux/makefile) to bring up the QEMU [AGL](https://www.automotivelinux.org/) environment.
* very simple and easy, with one single [Makefile](https://github.com/parai/as/blob/master/release/aslinux/script/vexpress-ca9.mk) to build out the kernel and the rootfs, and a [shell script](https://github.com/parai/as/blob/master/release/aslinux/script/run-vexpress.sh) to kick off the qemu virtual machine to execute that kernel and mount that rootfs.(for details check my [github.io page](http://parai.github.io/as/navigations/categories.html) on category linux)
* aslinux build step

```sh
git clone https://github.com/parai/as.git
cd $(patch-to)/as/release/aslinux
mkdir build
cd build
ln -fs ../script/vexpress-ca9.mk Makefile
ln -fs ../script/run-vexpress.sh run.sh
make all
./run.sh
# if aslua want to be used
cd $(path-to)/as/release/aslua && make 31 && make 33 
cp out/arm.exe $(path-to)/as/release/aslinux/build/out/rootfs/usr/bin/aslua -v
cp /usr/arm-linux-gnueabi/lib/*.so* $(path-to)/as/release/aslinux/build/out/rootfs/lib -v
# then the sdcard need to be rebuilt
cd $(path-to)/as/release/aslinux/build && make sdcard
```

## 7. Other 3rd part package
* [LWIP](http://savannah.nongnu.org/projects/lwip/)
* [afbindef/websock](https://github.com/automotive-grade-linux/docs-agl/blob/master/docs/app-framework/index.md)
* qemu PCI sample driver: [asnet](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/asnet.c) [ascan](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/ascan.c) [asblk](https://github.com/parai/as/blob/master/com/as.tool/qemu/hw/char/asblk.c)
* [FatFS](http://elm-chan.org/fsw/ff/00index_e.html)
* [lwext4](https://github.com/gkostka/lwext4.git)
* libelf: an OS independent ELF library for ELF loading&executing.

## 8. Setup Environment

* check the [asenv repository](https://github.com/parai/asenv) for win32 development

* for windows x64, follow below steps

### 8.1 install [python27 64bit](https://www.python.org) as C:\Python27

### 8.2 install [scons 3.0.0 zip](http://scons.org/pages/download.html) for python27

```sh
cd scons-3.0.0
C:\Python27\python.exe setup.py install
```

### 8.3 install [Anaconda3 64bit](https://www.anaconda.com/download/) as C:\Anaconda3

### 8.4 install [msys2 x86_64](http://www.msys2.org/) as C:\msys64

### 8.5 then run below commands in cmd

```sh
set PATH=C:\Python27;C:\Anaconda3\Scripts;C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%
pacman -Sy
pacman -S unzip wget git mingw-w64-x86_64-gcc mingw-w64-x86_64-glib2 mingw-w64-x86_64-gtk3
pacman -S ncurses-devel gperf scons
pacman -Syuu
wget https://pypi.python.org/packages/1f/3b/ee6f354bcb1e28a7cd735be98f39ecf80554948284b41e9f7965951befa6/pyserial-3.2.1.tar.gz#md5=7142a421c8b35d2dac6c47c254db023d
tar xf pyserial-3.2.1.tar.gz
cd pyserial-3.2.1
C:/Anaconda3/python setup.py install
```

## 9. Build

* launch the [/as/Console.bat](https://github.com/parai/as/blob/master/Console.bat) as Administrator

```sh
# in the cmd of aslua
make aslua
# in the cmd of asboot or ascore
scons # build, it will hint you the boards supported
scons run
scons -c # clean
# in the cmd of as.one.py
python main.py
```
