---
layout: post
title: aslinux GPIO system
category: linux
comments: true
---

GPIO - General Purpose Input Output

This is a very important part to most of the embeded system as we need IO pins to do some controls of the exeternal world. By the last post [aslinux-dts-study](http://parai.github.io/as/linux/2016/02/20/aslinux-dts-study.html), I think I have already known the basic points of the device tree system. So I think it would be much better if I can develop one driver with DTS, and I think GPIO driver is a very good entry point. But as I have no real board, so I choose use QEMU vexpress-a9 as a simulation platform, let's start now.

1. New file [kernel/drivers/pinctrl/pinctrl-asvirt.c](https://github.com/parai/as/blob/master/release/aslinux/kernel/drivers/pinctrl/pinctrl-asvirt.c)

2. apply the patch below

```patch
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
index c6a66de..2c8223c 100644
--- a/drivers/pinctrl/Kconfig
+++ b/drivers/pinctrl/Kconfig
@@ -223,6 +223,12 @@ config PINCTRL_TB10X
        bool
        depends on ARC_PLAT_TB10X
 
+config ASLINUX_VIRT_GPIO
+    bool "aslinux virtual gpio driver"
+    depends on ARCH_VEXPRESS
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

There is something wrong the driver register API is not called