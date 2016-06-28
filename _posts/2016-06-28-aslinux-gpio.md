---
layout: post
title: aslinux GPIO system
category: linux
comments: true
---

GPIO - General Purpose Input Output

This is a very important part to most of the embeded system as we need IO pins to do some controls of the exeternal world. By the last post [aslinux-dts-study](http://parai.github.io/as/linux/2016/02/20/aslinux-dts-study.html), I think I have already known the basic points of the device tree system. So I think it would be much better if I can develop one driver with DTS, and I think GPIO driver is a very good entry point. But as I have no real board, so I choose use QEMU vexpress-a9 as a simulation platform, let's start now.

1. New file [kernel/drivers/misc/pinctrl-asvirt.c](https://github.com/parai/as/blob/master/release/aslinux/kernel/drivers/misc/pinctrl-asvirt.c)

2. apply the patch below

```patch
diff --git a/drivers/misc/Kconfig b/drivers/misc/Kconfig
index bbeb451..d1c4924 100644
--- a/drivers/misc/Kconfig
+++ b/drivers/misc/Kconfig
@@ -515,6 +515,12 @@ config VEXPRESS_SYSCFG
          bus. System Configuration interface is one of the possible means
          of generating transactions on this bus.
 
+config ASLINUX_VIRT_GPIO
+    bool "aslinux virtual gpio driver"
+    depends on ARCH_VEXPRESS
+    help
+      aslinux virtual gpio driver sample code
+
 source "drivers/misc/c2port/Kconfig"
 source "drivers/misc/eeprom/Kconfig"
 source "drivers/misc/cb710/Kconfig"
diff --git a/drivers/misc/Makefile b/drivers/misc/Makefile
index 7d5c4cd..00518ed 100644
--- a/drivers/misc/Makefile
+++ b/drivers/misc/Makefile
@@ -56,3 +56,6 @@ obj-$(CONFIG_GENWQE)          += genwqe/
 obj-$(CONFIG_ECHO)             += echo/
 obj-$(CONFIG_VEXPRESS_SYSCFG)  += vexpress-syscfg.o
 obj-$(CONFIG_CXL_BASE)         += cxl/
+
+obj-$(CONFIG_ASLINUX_VIRT_GPIO)        += pinctrl-asvirt.o
```

There is something wrong the driver register API is not called