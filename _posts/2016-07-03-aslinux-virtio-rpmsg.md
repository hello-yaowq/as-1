---
layout: post
title: aslinux virtio rpmsg
category: linux
comments: true
---

Okay, today on the way to the book store to buy a Japenese study book, I came up a thought that how to study virtio/rpmsg on qemu though without a real board with multi-core(yes, I know with [qemu-tz](http://www.linaro.org/blog/core-dump/arm-trustzone-qemu/) it was possible, but it was too complicated for me), that is a simulation of 2 drivers, one for linux side virtio and the other one still run as a linux driver but with simulation behavior of the remote processor of the linux, generally it behaves like OMAP M3 core. As in real world such as OMAP CPU, it was the ISR used to notify the event between the 2 cores, so in the simulation world, each of the driver needs a thread and a sempahore to simulate the ISR.

And now the good point is that I have already done a simple [virtio/rpmsg](https://github.com/parai/as/tree/master/com/as.infrastructure/communication/RPmsg) implementation for MCU which runs on windows host system with almost the same simulation method described above, but it was dedicated for MCU, but now I want to learn about linux side virtio/rpmsg, so in generally, I have almost have half of the things done.

I think this was a good idea, let me do more research and if possible then kick this idea off.

researched, it was possible, so start to kick this idea off.

1. apply the below patch

```patch
diff --git a/arch/arm/mach-vexpress/Kconfig b/arch/arm/mach-vexpress/Kconfig
index 4f7a164..eb83a25 100644
--- a/arch/arm/mach-vexpress/Kconfig
+++ b/arch/arm/mach-vexpress/Kconfig
@@ -21,6 +21,7 @@ menuconfig ARCH_VEXPRESS
        select VEXPRESS_SYSCFG
        select MFD_VEXPRESS_SYSREG
        select PINCTRL
+       select HAS_DMA
+       select SAMPLES
        help
          This option enables support for systems using Cortex processor based
          ARM core and logic (FPGA) tiles on the Versatile Express motherboard,
diff --git a/drivers/remoteproc/Kconfig b/drivers/remoteproc/Kconfig
index 5e343ba..1999738 100644
--- a/drivers/remoteproc/Kconfig
+++ b/drivers/remoteproc/Kconfig
@@ -64,4 +64,15 @@ config DA8XX_REMOTEPROC
          It's safe to say n here if you're not interested in multimedia
          offloading.
 
+config ASVIRT_REMOTEPROC
+       tristate "ASLINUX virtual remoteproc support"
+       depends on ARCH_VEXPRESS
+       select REMOTEPROC
+       select RPMSG
+       help
+         Say y here to support ASLINUX's virtual remote processors 
+         via the remote processor framework, the 2 processors core
+         and MCU are both simulated by threads.
+
 endmenu
+
diff --git a/drivers/remoteproc/Makefile b/drivers/remoteproc/Makefile
index ac2ff75..1fa03d1 100644
--- a/drivers/remoteproc/Makefile
+++ b/drivers/remoteproc/Makefile
@@ -10,3 +10,4 @@ remoteproc-y                          += remoteproc_elf_loader.o
 obj-$(CONFIG_OMAP_REMOTEPROC)          += omap_remoteproc.o
 obj-$(CONFIG_STE_MODEM_RPROC)          += ste_modem_rproc.o
 obj-$(CONFIG_DA8XX_REMOTEPROC)         += da8xx_remoteproc.o
+obj-$(CONFIG_ASVIRT_REMOTEPROC)                += asvirt_remoteproc.o
+obj-$(CONFIG_ASVIRT_REMOTEPROC)                += rproc-asvirt/
```

1. New file [kernel/drivers/remoteproc/asvirt_remoteproc.c](https://github.com/parai/as/blob/1bb1e9118b28afbec4330e2768a7418a74811827/release/aslinux/kernel/drivers/remoteproc/asvirt_remoteproc.c)

2. update the DTS [kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi](https://github.com/parai/as/blob/1bb1e9118b28afbec4330e2768a7418a74811827/release/aslinux/kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi)

so that an initialization of remoteproc driver is done, next step is to study to know how to start up a background thread task that to simulated the IPC(Inter Processor Communication) hardware controller.

* chekc [kernel/drivers/remoteproc/asvirt_remoteproc.c](https://github.com/parai/as/blob/126facc16c1af18ba81877685abb5dbc7232840f/release/aslinux/kernel/drivers/remoteproc/asvirt_remoteproc.c), the 2 thread and its mutex and semaphore are all done, things go very well.

then adding the code of MCU which run in the thread of MCU which do simulation of the behavior of the remote processor, check [kernel/drivers/remoteproc/rproc-asvirt](https://github.com/parai/as/blob/e95df67a574ccb1f1da399254b304f2aaaf2ec15/release/aslinux/kernel/drivers/remoteproc/rproc-asvirt).
 
done the coding with [this commit](https://github.com/parai/as/tree/6d6f2e5359bf0624a09c6fa4a666e31708474624), and then try the rpmsg sample client of the kernel samples, make menuconfig, under kernel hacking code, enable the RPMSG sample client then rebuild. But things not going well, headache, have a rest and then study it again tomorrow.

[2016-7-17]till now, the rpmsg bus can be successfully probed and it kick a idx to the mcu side thread sucessfully, but as the ring information is passed by the member of resource table da, I don't know how to change the address da to an address that the mcu thread can access, now I use API phys\_to\_virt, but it seems that it gives a wrong address, I can't get the right data content of ring descriptor as initialized by API vring\_new\_virtqueue of virtio\_ring.c. This is a block point now, if I can solve this problem, then all issue is fixed. I think the API dma\_alloc\_coherent is the key point, I should know the relation ship between va and dma.

* [rproc-asvirt/Makfile](https://github.com/parai/as/blob/master/release/aslinux/kernel/drivers/remoteproc/rproc-asvirt/Makefile): This is a makefile that can be used to prepare the code on MCU side, just run command "make dep" under the same directory of the Makefile.

* [aslinux/script/vexpress-ca9.mk](https://github.com/parai/as/blob/master/release/aslinux/script/vexpress-ca9.mk): okay, this is a automatic makefile that to prepare the code kernel and busybox and so on, just run command "make all", enjoy it.


