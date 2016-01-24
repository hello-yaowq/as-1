# as
automotive software and its tool-chain

## why this repository
because I am not powerful so I decided to develop small but smart part of automotive software.

## Status
2016-Jan-24:  Now I have porting toppers-atk2-sc4 on arm920t(s3c2440a) and arm-cortex-m3, but now I feel confused about what to do for the next step. 

## Key Point

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

## 3. BSW configuration tool
> This GUI tool is the one from [OpenSAR](https://github.com/parai/OpenSAR.git) which is also developed by me, the GUI can be automatically created from xml.

## 4. Virtual Simulation of AUTOSAR
> Qt simulator, rpmsg strategy for the ECUs(dll) connectivity simulation 
* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)
* [README](./com/as.infrastructure/system/vm/readme.md)

## 5. Lua Script Integrated
* support CAN device: CANcaseXL, [PeakCAN](http://www.peak-system.com/PCAN-USB.199.0.html?L=1), [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN),also provide a kind of python library for CAN access.

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

   
    

