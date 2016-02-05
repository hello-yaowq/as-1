---
layout: post
title: Automotive linux overview
category: linux
comments: true
---

## Automotive Software on Linux -- [aslinux](https://github.com/parai/as/release/aslinux)
Yes, there is AGL(Auto-Graded-Linux) and GENIVI(General In Vehivle Infortainment system based on linux), so it's better to study AGL or GENIVI dirctly, but as an infrastructure software developer, I choose to new the aslinux release for the purpose to study linux system deeply and learn a way how to use linux as Automotive Software platform.

Now I almost has no idea of linux operating system, but as I think I have a very good knowledge of RTOS, I think it should be easy to know how linux operating system works if I am willing to read the fucking source code directly. I have done some basic research and I have found out that the qemu can simulate arm machine vexpress-a9 very well, and there is already some articles to teach us how to setup the software environment and build the rootfs and create a sdcard image and then the qemu could run the linux with that sdcard as file system. I think this is good for me, I can do research without buying a real board.

So I following the instructions on net, and create a simple makefile [mk-vexpress.mk](https://github.com/parai/as/release/aslinux/script/mk-vexpress.mk) that can download the necessay packages and then do build job and generate the rootfs. And now I have successfully use the sacard image with the rootfs to start the qemu machine.

check the startup image as below, now only 4 packages used: u-boot linux busybox and glibc.
![qemu-vexpress-a9](/as/images/vexpress-a9/qemu-vexpress-a9-startup.png)
As you see there is something wrong as no file "/etc/init.d/rcS" to specify the initialization job, so the next step for me is that I will study the linux initialization system and fix the issue above.

