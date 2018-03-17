---
layout: post
title: AUTOSAR Overview
category: AUTOSAR
comments: true
---

## AUTOSAR - Infrastructure

AUTOSAR (AUTomotive Open System ARchitecture) is a worldwide development partnership of vehicle manufacturers, suppliers and other companies from the electronics, semiconductor and software industry.

The below directory tree of as/com list the component/tools has been integrated into the repository [as](https://github.com/parai/as).

```c
AS
├── com
│   ├── as.application
│   │   ├── board.at91sam3s
│   │   ├── board.bcm2835
│   │   ├── board.lm3s6965evb
│   │   ├── board.posix
│   │   ├── board.s3c2440a
│   │   ├── board.stm32f107vc
│   │   ├── board.versatilepb
│   │   ├── board.virt
│   │   ├── board.x86
│   │   ├── common
│   │   │   ├── config
│   │   │   ├── rte
│   │   │   └── test
│   │   └── swc
│   │       └── telltale
│   ├── as.infrastructure
│   │   ├── arch
│   │   │   ├── at91sam3s
│   │   │   ├── bcm2835
│   │   │   ├── common
│   │   │   │   ├── lwip
│   │   │   │   └── mcal
│   │   │   ├── lm3s
│   │   │   ├── posix
│   │   │   ├── s3c2440a
│   │   │   ├── stm32f1
│   │   │   ├── versatilepb
│   │   │   ├── virt
│   │   │   └── x86
│   │   ├── boot	/* PBL on XCP and XBL on UDS */
│   │   │   └── common
│   │   ├── clib
│   │   │   └── newlib
│   │   ├── communication
│   │   │   ├── CanIf
│   │   │   ├── CanNm
│   │   │   ├── CanSM
│   │   │   ├── CanTp
│   │   │   ├── Com
│   │   │   ├── ComM
│   │   │   ├── DoIP
│   │   │   ├── Eth
│   │   │   ├── EthIf
│   │   │   ├── EthSM
│   │   │   ├── EthTrcv
│   │   │   ├── J1939Tp
│   │   │   ├── Lin
│   │   │   ├── Nm
│   │   │   ├── OsekNm
│   │   │   ├── Pci
│   │   │   ├── PduR
│   │   │   ├── RPmsg	/* Remote Processor Message */
│   │   │   ├── SD
│   │   │   ├── SoAd
│   │   │   ├── UdpNm
│   │   │   └── Xcp
│   │   ├── diagnostic
│   │   │   ├── Dcm
│   │   │   ├── Dem
│   │   │   └── Det
│   │   ├── include
│   │   ├── libraries
│   │   │   └── Ifx
│   │   ├── memory
│   │   │   ├── Ea
│   │   │   ├── Fee
│   │   │   ├── MemIf
│   │   │   └── NvM
│   │   └── system
│   │       ├── BswM
│   │       ├── Crc
│   │       ├── EcuM
│   │       ├── SchM
│   │       ├── Tm
│   │       ├── cplusplus
│   │       ├── fs
│   │       │   ├── fatfs
│   │       │   ├── libdl
│   │       │   ├── libelf
│   │       │   ├── lwext4
│   │       │   └── vfs
│   │       ├── gui
│   │       │   └── openvg
│   │       ├── kernel
│   │       │   ├── askar
│   │       │   ├── contiki
│   │       │   ├── freeosek
│   │       │   ├── freertos
│   │       │   ├── posix
│   │       │   ├── rtthread
│   │       │   ├── small
│   │       │   ├── toppers-atk
│   │       │   ├── toppers_osek
│   │       │   ├── trampoline
│   │       │   └── ucos_ii
│   │       ├── net
│   │       │   ├── ftp
│   │       │   └── lwip
│   │       ├── shell
│   │       └── vm
│   │           ├── rpmsg
│   │           ├── rproc
│   │           └── virtio
│   └── as.tool
│       ├── as.one.py
│       ├── cancasexl.access
│       ├── config.autosar.bsw
│       ├── config.infrastructure.gui
│       ├── config.infrastructure.system
│       ├── kconfig-frontends
│       ├── lua
│       ├── py.can.database.access
│       └── qemu
└── release
    ├── asboot
    ├── ascore
    ├── askar
    └── aslua
```

### 1. Integrated ArcCore AUTOSAR 3.1 BSW
* Communication: Can(CanFD) CanIf PduR Com SoAd
* Diagnostic: CanTp Dcm DoIP Dem
* System: EcuM SchM
* Memory: Fls Fee Eep Ea NvM MemIf

### 2. OS [ASKAR](https://github.com/parai/as/tree/master/com/as.infrastructure/system/kernel/askar) - [Automotive oSeK AutosaR](http://parai.github.io/as/autosar/2017/10/27/ASKAR-architecture.html)
This RTOS ASKAR is designed according to [OSEK/VDX OS223](http://trampolinebin.rts-software.org/os223.pdf) by taking a reference of a lot of other open source RTOS, generally, the bwlow is the list:

* [toppers_osek](https://www.toppers.jp/osek-os.html)
* [FreeOSEK](http://opensek.sourceforge.net/)
* [FreeRTOS](http://www.freertos.org/)
* [ucos ii](https://www.micrium.com/)
* [toppers-atk2](https://www.toppers.jp/atk2.html)
* [contiki](http://contiki-os.org/)
* [trampoline](https://github.com/TrampolineRTOS/trampoline)
* [rtthread](https://github.com/RT-Thread/rt-thread)

For the purpose that to be able to run most of the common applications which are based on posix, a series of posix APIs have been implemented based on the ASKAR OSEK tiny core.

* posix threads
* posix semaphore
* posix mutex and condition
* posix message queue
* posix signals

### 3. BSW configuration tool config.infrastructure.system
> This GUI tool is the one copied from [OpenSAR](https://github.com/parai/OpenSAR.git) which is also developed by me, the GUI can be automatically created from xml.

![config.infrastructure.system](/as/images/config.infrastructure.system.gif)

### 4. AUTOSAR & LINUX communication
> mostly for the purpose to study linux virtio/rpmsg system

* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)


### 5. Lua & Python Integrated
* support CAN device by lascanlib or pyas: CANcaseXL, [PeakCAN](http://www.peak-system.com/PCAN-USB.199.0.html?L=1), [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN), [ZLG USBCAN](http://www.zlg.cn/can/can/product/id/22.html)
* support miscellaneous device by lasdevlib with the smae API interface "open/read/write/ioctl/close": rs232
* a powerful python as.one.py tool: Dcm Xcp Bootloader and others.

![as.one.py bootloader](/as/images/python3-asone-tool.png)

## AUTOSAR - Release

This kind of release are based on the as/com infrastructure software. The below is the tree directory.

```c
as/release/
├── asboot	/* as sample bootloader */
├── ascore	/* as sample application core */
└── aslua	/* as lua and python tool release */
```

### 1. build

For how to set up the development environment, please check [as-study-platform](http://parai.github.io/as/autosar/2018/02/20/as-study-platform.html).

## The End
By this as/com package, it is very freindly to use it to study AUTOSAR and its toolchain, I hope this repository could help others and others could do contributions to this repository and make this repository famous.
