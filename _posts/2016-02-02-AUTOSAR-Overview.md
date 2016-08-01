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
as/com/
├── as.application
│   ├── board.at91sam3s
│   ├── board.lm3s6965evb
│   ├── board.posix
│   ├── board.s3c2440a
│   ├── board.stm32f107vc
│   └── common
├── as.infrastructure
│   ├── arch
│   │   ├── at91sam3s
│   │   ├── lm3s
│   │   ├── posix
│   │   ├── s3c2440a
│   │   └── stm32f1
│   ├── boot
│   │   └── common
│   ├── clib
│   ├── communication	/* Communication Stack */
│   │   ├── CanIf
│   │   ├── CanNm
│   │   ├── CanSM
│   │   ├── CanTp
│   │   ├── Com
│   │   ├── ComM
│   │   ├── J1939Tp
│   │   ├── Lin
│   │   ├── Nm
│   │   ├── PduR
│   │   └── RPmsg		/* Remote Processor Message */
│   ├── diagnostic
│   │   ├── Dcm
│   │   ├── Dem
│   │   └── Det
│   ├── include
│   │   └── sys
│   ├── memory		/* Memory Stack */
│   │   ├── Ea
│   │   ├── Fee
│   │   ├── MemIf
│   │   └── NvM
│   └── system
│       ├── Crc
│       ├── EcuM
│       ├── gui
│       ├── kernel
│       │   ├── freeosek
│       │   ├── freertos
│       │   ├── small
│       │   ├── toppers-atk
│       │   └── toppers_osek
│       ├── SchM
│       └── shell
├── as.tool
│   ├── as.one.py
│   ├── cancasexl.access
│   ├── config.infrastructure.gui
│   ├── config.infrastructure.system
│   ├── lua
│   └── py.can.database.access
└── as.virtual
```

### 1. Integrated ArcCore AUTOSAR 3.1 BSW
* Communication: Can CanIf PduR Com 
* Diagnostic: CanTp Dcm
* System: EcuM SchM
* Memory: Fls Fee Eep Ea NvM MemIf

### 2. OS 
* [toppers_osek](https://www.toppers.jp/osek-os.html)
* [FreeOSEK](http://opensek.sourceforge.net/)
* [FreeRTOS](http://www.freertos.org/)
* [toppers-atk2](https://www.toppers.jp/atk2.html)
* small : this OS is developed by me, OSEK BCC1 conformed

### 3. BSW configuration tool config.infrastructure.system
> This GUI tool is the one copied from [OpenSAR](https://github.com/parai/OpenSAR.git) which is also developed by me, the GUI can be automatically created from xml.

```sh
parai@UX303LB:~/workspace/as/release/asboot$ make studio board=posix compiler=posix-gcc
```

![config.infrastructure.system](/as/images/config.infrastructure.system.png)

### 4. AUTOSAR & LINUX communication
> mostly for the purpose to study linux virtio/rpmsg system

* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)


### 5. Lua Script Integrated
* support CAN device: CANcaseXL, [PeakCAN](http://www.peak-system.com/PCAN-USB.199.0.html?L=1), [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN),also provide a kind of python library for CAN access.

## AUTOSAR - Release

This kind of release are based on the as/com infrastructure software, and the project files are dynamic linked by the makefile when build. The below is the tree directory.

```c
as/release/
├── asboot	/* as sample bootloader */
├── ascore	/* as sample application core */
├── aslinux	/* as linux release */
├── aslua	/* as lua and python tool release */
└── make	/* makefile system */
```

### 1. build
under each directory asboot,ascoroe or aslua, there is a Makefile, it can be used to build each release. take the asboot for example, the below is snippet of the Makefile.

```makefile
# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=cortexm3-gcc
12:
	make all board=stm32f107vc compiler=cortexm3-gcc debug=true
13:
	make all board=stm32f107vc compiler=cortexm3-gcc debug=false	
	
15:
	make dep board=stm32f107vc compiler=cortexm3-icc
16:
	make all board=stm32f107vc compiler=cortexm3-icc debug=true
17:
	make all board=stm32f107vc compiler=cortexm3-icc debug=false
	
# ---------------------------------- [ STM32F107VC-FLSDRV ] ---------------------------------- #
21:
	make dep board=stm32f107vc-flsdrv compiler=cortexm3-gcc
22:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-gcc debug=true
23:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-gcc debug=false	
	
25:
	make dep board=stm32f107vc-flsdrv compiler=cortexm3-icc
26:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-icc debug=true
27:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-icc debug=false
	
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc
92:
	make all board=posix compiler=posix-gcc debug=true
93:
	make all board=posix compiler=posix-gcc debug=false	
94:
	make dll board=posix compiler=posix-gcc debug=true	
	@mv -v $(exe-dir)/posix.dll $(exe-dir)/boot.dll
``` 

For each board make, it was divided in to 2 steps, the first step should be "make dep" to link the infrastructure files to create a project, and then the second step should be "make all". For example for the posix board:

```shell
parai@UX303LB:~$ cd workspace/as/release/asboot/
parai@UX303LB:~/workspace/as/release/asboot$ make 91
	......
    >>> Gen Can DONE <<<
    >>> Gen CanIf DONE <<<
    >>> Gen CanTp DONE <<<
    >>> Gen PduR DONE <<<
    >>> Gen Dcm DONE <<<
make[2]: Leaving directory `/home/parai/workspace/as/release/asboot'
  >> prepare link for posix done
  KsmGen ...
  OsGen ...
  >> XCC /home/parai/workspace/as/release/asboot/src/posix-posix-gcc/*.xml done.
make[1]: Leaving directory `/home/parai/workspace/as/release/asboot'
parai@UX303LB:~/workspace/as/release/asboot$ make 92
  ......
  >> LD posix.exe
>>>>>>>>>>>>>>>>>  BUILD /home/parai/workspace/as/release/asboot/out/posix  DONE   <<<<<<<<<<<<<<<<<<<<<<
make[1]: Leaving directory `/home/parai/workspace/as/release/asboot'
```

then you can use the asboot/out/posix.exe to debug the asboot.

```shell
parai@UX303LB:~/workspace/as/release/asboot$ sudo modprobe vcan
[sudo] password for parai: 
parai@UX303LB:~/workspace/as/release/asboot$ sudo ip link add dev can0 type vcan
parai@UX303LB:~/workspace/as/release/asboot$ sudo ip link set up can0
parai@UX303LB:~/workspace/as/release/asboot$ out/posix.exe 
 start bootloaster BUILD @ Feb 11 2016 09:50:38
parai.STDOUT     :: can set on-line!
```

start the lua bootloader tool to test the bootloader

```shell
# start a log server to show the CAN bus message trace
parai@UX303LB:~/workspace/as/com/as.tool/lua/script$ ./aslua.exe canlogserver.lua socket
 >> canlog server on-line!
canid=731 dlc=8 data=[02,10,03,55,55,55,55,55,] @ 86.115254 s
canid=732 dlc=8 data=[06,50,03,00,64,00,0A,00,] @ 86.129062 s
canid=731 dlc=8 data=[02,27,01,55,55,55,55,55,] @ 86.129671 s
canid=732 dlc=8 data=[06,67,01,2B,DA,4A,7D,00,] @ 86.148439 s
canid=731 dlc=8 data=[06,27,02,53,49,0C,0E,55,] @ 86.148533 s
canid=732 dlc=8 data=[02,67,02,00,00,00,00,00,] @ 86.170726 s


# start the lua flashloader tool
parai@UX303LB:~/workspace/as/com/as.tool/lua/script$ ./aslua.exe flashloader.lua socket
[sudo] password for parai: 
RTNETLINK answers: File exists
parai.STDOUT     :: can trace log to file < laslog/flash-loader.asc >

  >> dcm request diagnostic session control = [10,03,],
  >> dcm response = [50,03,00,64,00,0A,],
  >> enter extend session ok!
  >> dcm request security access = [27,01,],
  >> dcm response = [67,01,2B,DA,4A,7D,],
  >> security access request seed ok!
number	1397296142
  >> dcm request security access = [27,02,53,49,0C,0E,],
  >> dcm response = [67,02,],
  >> security access send key ok!
  >> dcm request diagnostic session control = [10,02,],
  >> dcm response = [50,02,00,64,00,0A,],
  >> enter program session ok!
  >> dcm request security access = [27,03,],
  >> dcm response = [67,03,20,42,D3,4A,],
  >> security access request seed ok!
  >> dcm request security access = [27,04,B4,1A,B4,D8,],
  >> dcm response = [67,04,],
  >> security access send key ok!
  >> dcm request request download = [34,00,44,00,00,00,00,00,00,05,A8,FD,],
  >> dcm response = [74,20,0F,FF,],
  >> request download ok!
  >> dcm request transfer data = [36,01,00,FD,12,45,00,01,19,00,00,20,59,00,00,20,],
  >> dcm response = [76,01,],
  >> dcm request request transfer exit = [37,],
  >> dcm response = [77,],
  >> request_transfer_exit ok!
  >> download flash driver ok!
  >> dcm request request upload = [35,00,44,00,00,00,00,00,00,05,A8,FD,],
  >> dcm response = [75,20,0F,FF,],
  >> request upload ok!
  >> dcm request transfer data = [36,01,00,FD,],
  >> dcm response = [76,01,12,45,00,01,19,00,00,20,59,00,00,20,71,00,],
  >> dcm request request transfer exit = [37,],
  >> dcm response = [77,],
  >> request_transfer_exit ok!
  >> check flash driver ok!
  >> dcm request routine control = [31,01,FF,01,00,01,00,00,00,03,00,00,FF,],
  >> dcm response = [7F,31,78,],
  >> dcm response = [71,01,FF,01,],
  >> routine erase flash ok!
  >> dcm request request download = [34,00,44,00,01,00,00,00,01,01,34,FF,],
  >> dcm response = [74,20,0F,FF,],
  >> request download ok!
  >> dcm request transfer data = [36,01,00,FF,DF,F8,2C,D0,0B,48,0C,4B,0C,4D,00,21,],
  >> dcm response = [76,01,],
	......
  >> dcm request routine control = [31,01,FF,03,],
  >> dcm response = [7F,31,22,],
  >> service 'routine control' negative response 'conditions not correct' 
  >> routine test jump to application failed!

```

also, there is a python implementation which is a GUI tool for the bootloader.

```sh
parai@UX303LB:~/workspace/as/com/as.tool/as.one.py$ python3 main.py 
```

![as.one.py bootloader](/as/images/python3-asone-tool.png)

## The End
By this as/com package, it is very freindly to use it to study AUTOSAR and its toolchain, I hope this repository could help others and others could do contributions to this repository and make this repository famous.