---
layout: post
title: aslinux GPIO&PINCTRL system
category: linux
comments: true
---

GPIO&PINCTRL - General Purpose Input Output and Pin mode control system

This is a very important part to most of the embeded system as we need IO pins to do some controls of the exeternal world. By the last post [aslinux-dts-study](http://parai.github.io/as/linux/2016/02/20/aslinux-dts-study.html), I think I have already known the basic points of the device tree system. So I think it would be much better if I can develop one driver with DTS, and I think GPIO driver is a very good entry point. But as I have no real board, so I choose use QEMU vexpress-a9 as a simulation platform, let's start now.

1. New file [kernel/drivers/pinctrl/pinctrl-asvirt.c](https://github.com/parai/as/blob/b75f6b010f775aa4e6a058dfb0ffabc921abb566/release/aslinux/kernel/drivers/pinctrl/pinctrl-asvirt.c)

2. New a simple DTS [kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi](https://github.com/parai/as/blob/b75f6b010f775aa4e6a058dfb0ffabc921abb566/release/aslinux/kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi)

3. apply the patch below

```patch
diff --git a/arch/arm/boot/dts/vexpress-v2p-ca9.dts b/arch/arm/boot/dts/vexpress-v2p-ca9.dts
index 23662b5..f8cae6b 100644
--- a/arch/arm/boot/dts/vexpress-v2p-ca9.dts
+++ b/arch/arm/boot/dts/vexpress-v2p-ca9.dts
@@ -356,4 +356,5 @@
 
                /include/ "vexpress-v2m.dtsi"
        };
+       /include/ "vexpress-v2p-asvirt.dtsi"
 };
diff --git a/arch/arm/mach-vexpress/Kconfig b/arch/arm/mach-vexpress/Kconfig
index b2cfba1..4f7a164 100644
--- a/arch/arm/mach-vexpress/Kconfig
+++ b/arch/arm/mach-vexpress/Kconfig
@@ -20,6 +20,7 @@ menuconfig ARCH_VEXPRESS
        select VEXPRESS_CONFIG
        select VEXPRESS_SYSCFG
        select MFD_VEXPRESS_SYSREG
+       select PINCTRL
        help
          This option enables support for systems using Cortex processor based
          ARM core and logic (FPGA) tiles on the Versatile Express motherboard,
diff --git a/drivers/pinctrl/Kconfig b/drivers/pinctrl/Kconfig
index c6a66de..7b83329 100644
--- a/drivers/pinctrl/Kconfig
+++ b/drivers/pinctrl/Kconfig
@@ -223,6 +223,14 @@ config PINCTRL_TB10X
        bool
        depends on ARC_PLAT_TB10X
 
+config ASLINUX_VIRT_GPIO
+    bool "aslinux virtual gpio driver"
+    depends on ARCH_VEXPRESS
+       select PINMUX
+       select GENERIC_PINCONF
+    help
+      aslinux virtual gpio driver sample code
+
 endmenu
 
 endif
diff --git a/drivers/pinctrl/Makefile b/drivers/pinctrl/Makefile
index 51f52d3..95562ac 100644
--- a/drivers/pinctrl/Makefile
+++ b/drivers/pinctrl/Makefile
@@ -47,3 +47,5 @@ obj-$(CONFIG_PINCTRL_SH_PFC)  += sh-pfc/
 obj-$(CONFIG_PLAT_SPEAR)       += spear/
 obj-$(CONFIG_ARCH_SUNXI)       += sunxi/
 obj-$(CONFIG_ARCH_VT8500)      += vt8500/
+obj-$(CONFIG_ASLINUX_VIRT_GPIO)        += pinctrl-asvirt.o
```

There is something wrong the driver register API is not called, by debug with QEMU with eclipse, it founds out that the printk message doesn't goes out directly, but with command dmesg, I can see the debug information, but I already set the loglevel to 7, the lowest, but it still not work. so who cares, let's use pr_debug.

So then on, take the pinctrl-sirf.c as an example. so almost copy the content of pinctrl-sirf, make the build pass, so according the content of dtsi, 3 pinctrl devices will be created and driver pinctrl-asvirt will be probed 3 times, this is a very small step forware, but also a big step, so let's go on to know how to fill the API up to make the virtual pinctrl driver works.

During the filling up of the driver API, I think the pinctrl-sirf is a mess, very ugly, but looking into the code of pinctrl-st, the driver seems to be very simple, but the dts is missing in the release package, okay, go on with the sirf, I think it was that the hardware is a mess.


okay, till 2016-7-3 12:09, it was done almost for all of the API, and I think I know some basic things of PINCTRL, okay, then I need to research how the API was called by other drivers or APP, below is the code now:

* [kernel/drivers/pinctrl/pinctrl-asvirt.c](https://github.com/parai/as/blob/22683e54839962571d4ae9e03fda315c00b212dd/release/aslinux/kernel/drivers/pinctrl/pinctrl-asvirt.c)

* [kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi](https://github.com/parai/as/blob/22683e54839962571d4ae9e03fda315c00b212dd/release/aslinux/kernel/arch/arm/boot/dts/vexpress-v2p-asvirt.dtsi)

