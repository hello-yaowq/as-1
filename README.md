# as
automotive software and its tool-chain

## why this repository
because I am not powerful so I decided to develop small but smart part of automotive software.

## Status
2015-July-28: Till now, FreeRTOS on MINGW for the purpose to simulate OSEK ECC2 OS partially OK. Toppers_osek is the real OSEK ECC2 RTOS for real hardware boards. By this OS simulation strategy, application can be developed with the real board ready. And as my power is very limited, I start to integrated ArcCore BSW(AUTOSAR 4.x).
2015-July-28: For the purpose to validate ArcCore, all of MCAL will be abstracted on the simulation platform FreeRTOS MINGW.
2015-Sep-18: roll back to accroe as I found out the the arccore is not free to be used in product.


## Idea
2015-Aug-17: I came up with an idea for how to implement the simulation of AUTOSAR BSW. That is compiling the AS core package
software to the format of DLL and then load the dll by the simulator. The simulator which should have rich feature to provide 
GUI or network connection, I think QT is perfect, even more, maybe I can integrated it with pyQt, much more wonderful, isn't 
it. Let's start.

## RPmsg
* [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
* [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
* [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
* [virtio ring buffer](http://www.ibm.com/developerworks/cn/linux/1402_caobb_virtio/)
* [README](./com/as.infrastructure/system/vm/readme.md)


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

   
    

