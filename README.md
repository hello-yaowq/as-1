# as
automotive software and its tool-chain

## why this repository
because I am not powerful so I decided to develop small but smart part of automotive software.

## Status
2015-July-28: Till now, FreeRTOS on MINGW for the purpose to simulate OSEK ECC2 OS partially OK. Toppers_osek is the real OSEK ECC2 RTOS for real hardware boards. By this OS simulation strategy, application can be developed with the real board ready. And as my power is very limited, I start to integrated ArcCore BSW(AUTOSAR 4.x).
2015-July-28: For the purpose to validate ArcCore, all of MCAL will be abstracted on the simulation platform FreeRTOS MINGW.


## Idea
2015-Aug-17: I came up with an idea for how to implement the simulation of AUTOSAR BSW. That is compiling the AS core package
software to the format of DLL and then load the dll by the simulator. The simulator which should have rich feature to provide 
GUI or network connection, I think QT is perfect, even more, maybe I can integrated it with pyQt, much more wonderful, isn't 
it. Let's start.

## RPmsg
   [virtio](http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.pdf)
   [remoteproc](https://www.kernel.org/doc/Documentation/remoteproc.txt)
   [rpmsg](https://www.kernel.org/doc/Documentation/rpmsg.txt)
   
    

