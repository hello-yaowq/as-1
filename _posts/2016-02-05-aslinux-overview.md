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

OK, let's see how a simple "hello world" program run on the vexpress-a9 linux.

```c
#include <stdio.h>
int main(int argc,char* argv[])
{
	printf("hello world\n");
	return 0;
}
```

save the code as main.c, and compiler it.

```sh
$ arm-linux-gnueabi-gcc main.c -o helloworld
```

then copy the bin file to the sdcard and invoke "make sdcard" to generate the sdcard image again, and then start the qemu emulator, on the shell of linux, run "./helloworld", you would see the print message "hello world", it is wonderfull, isn't it?

Now on we have a wonderfull emulator to take a deep look of the linux world, let's start together.

First, create a simple rcS file do initialization job to fix the error that no file "/etc/init.d/rcS".

```sh
echo "  >> welcome to the world of aslinux <<"
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:
```

OK, then on, let's see how to write a module driver that would say "hello world" when do insmod, the below is the module source code.

```c
#include <linux/kernel.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");
static int __init aslinux_hello_module_init(void)
{
	printk("Hello, aslinux hello module is installed !\n");
	return 0;
}
static void __exit aslinux_hello_module_cleanup(void)
{
	printk("Good-bye, aslinux hello module was removed!\n");
}
module_init(aslinux_hello_module_init);
module_exit(aslinux_hello_module_cleanup);
```

save it as kernel/drivers/char/aslinux_hello.c and add it to kernel menuconfig, edit kernel/drivers/char/Kconfig, add a item as below

```sh
config ASLNUX_HELLO_MODULE
	tristate "aslinux hello world module sample"
	depends on ARCH_VEXPRESS
	help
	  aslinux hello world module sample
```

then do menuconfig, under menu Device Drivers/Charactor devices, select the new add item as option \<M\>. and modify the kernel/drivers/char/Makefile, add "obj-$(CONFIG_ASLNUX_HELLO_MODULE)	+= aslinux_hello.o", then rebuild the kernel, then you would find out the moudle file by command 'find . -name "*.ko"', copy to rootfs, then when you do insmod, you would see the related print out infromation.(note, the directory "rootfs/lib/modules/3.18.0+" should be created manually)

```sh
/example # insmod aslinux_hello.ko
Hello, aslinux hello module is installed !
/example # rmmod aslinux_hello.ko
Good-bye, aslinux hello module was removed!
```
