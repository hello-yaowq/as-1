# as
automotive software and its tool-chain

[![Build Status](https://travis-ci.org/parai/as.svg?branch=master)](https://travis-ci.org/parai/as)

## why this repository
because I am not powerful so I decided to develop small but smart part of automotive software.

## Key Point

### 1. Integrated ArcCore AUTOSAR 3.1 BSW
* Communication: Can CanIf PduR Com SoAd
* Diagnostic: CanTp Dcm DoIP
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
* [ASKAR](https://github.com/parai/as/tree/master/com/as.infrastructure/system/kernel/askar) : Automotive OseK AutosaR designed by me, [OSEK/VDX OS223](http://trampolinebin.rts-software.org/os223.pdf) confirmed.

## 3. BSW configuration tool
> This GUI tool is the one from [OpenSAR](https://github.com/parai/OpenSAR.git) which is also developed by me, the GUI can be automatically created from xml.

![as configure tool](http://parai.github.io/as/images/config.infrastructure.system.png)

## 4. AUTOSAR & LINUX communication
* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)

## 5. Lua & Python Integrated
* support CAN device by lascanlib or pyas: CANcaseXL, [PeakCAN](http://www.peak-system.com/PCAN-USB.199.0.html?L=1), [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN),also provide a kind of python library for CAN access.
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

## Setup Environment

``` bash
sudo apt-get install git
sudo apt-get install libxext6 libxext-dev libqt4-dev libqt4-gui libqt4-sql qt4-dev-tools qt4-doc qt4-designer qt4-qtconfig "python-qt4-*" python-qt4
sudo apt-get install pyqt5-dev pyqt5-dev-tools
sudo apt-get install libreadline6 libreadline6-dev
sudo apt-get install openjdk-7-jre
sudo apt-get install build-essential
sudo apt-get install gcc-arm-none-eabi
sudo apt-get install gcc-arm-linux-gnueabi
sudo apt-get insatll g++-arm-linux-gnueabi
sudo apt-get install libncurses5-dev
sudo apt-get install gawk autoconf
sudo apt-get install zlib1g-dev libglib2.0-0 libglib2.0-dev
sudo apt-get install libsdl-dev
sudo apt-get install libgtk2.0-dev libgtk-3-dev
sudo apt-get install lib32z1 lib32ncurses5 lib32bz2-1.0 lib32stdc++6
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf  tslib
sudo apt-get install automake autogen autoconf libtool
sudo apt-get install u-boot-tools
sudo apt-get install nfs-kernel-server
sudo apt-get install mtd-utils
sudo apt-get install Kolourpaint
sudo apt-get install cmake uuid-dev libboost-dev libwebsockets-dev libjson-c-dev libjson-glib-1.0-0 libjson0 libjson0-dev
sudo apt-get install gcc-aarch64-linux-gnu
sudo apt-get install libstdc++6:i386 # for 64bit machine run 32bit app
sudo add-apt-repository ppa:webupd8team/java && sudo apt-get update && sudo apt-get install oracle-java7-installer
sudo apt-get install gtk-doc-tools
```

## Clone
> git clone https://github.com/sics-sse/moped.git

> git clone https://github.com/openxc/bitfield-c.git

> git clone git://git.omapzoom.org/repo/sysbios-rpmsg.git

> git clone https://github.com/parai/freertos-multicore.git

> git clone https://github.com/parai/isotp-c.git

> git clone https://github.com/parai/GainOS.git

> git clone https://github.com/parai/GainOS_Studio.git

> git clone https://github.com/parai/OpenOSEK.git

> git clone https://github.com/parai/OpenSAR.git

> git clone https://github.com/parai/WhatsApp.git

> git clone https://github.com/parai/as.git

> git clone https://github.com/parai/GaInOS_Based_On_uTenux-tkernel.git

> git clone https://github.com/parai/gainos-tk.git

> git clone https://github.com/lua/lua.git

> git clone https://github.com/z2akhtar/PWC-Demonstrator.git

> git clone https://github.com/elupus/autosar-xcp.git


