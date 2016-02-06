# make file to build the platform for mini2440
# start from a clean directory, 
# ln -fs /as/release/aslinux/script/mini2440.mk makefile

export ARCH=arm

gcc-vivi = $(CURDIR)/opt/usr/loca/arm/2.95.3
gcc-kernel = $(CURDIR)/opt/usr/local/arm/3.4.1
gcc-qt = $(CURDIR)/opt/usr/local/arm/3.3.2
gcc-common = $(CURDIR)/opt/usr/local/arm/3.4.1

out=$(CURDIR)/out
rootfs = $(out)/rootfs
download = $(CURDIR)/download

# first default make
all:$(rootfs) askernel asuboot asglibc asbusybox sdcard
	@echo "  >> build mini2440 done <<"

$(rootfs):
	@mkdir -p $(rootfs)
	@mkdir -p $(rootfs)/lib/modules
	@mkdir -p $(rootfs)/example

extract-kernel:
	@tar -zxvf $(download)/linux-2.6.13-qq2440-20080625.tgz -C $(CURDIR)
	@mv kernel-2.6.13 kernel

kernel/.config:
	@(cd kernel;cp config_n35 .config)
	@(cd kernel;make menuconfig O=.)

kernel-menuconfig:
	@(cd kernel;make menuconfig O=.)

askernel: kernel/.config
	@(cd kernel; make all CROSS_COMPILE=$(gcc-kernel)/bin/arm-linux-)
	@cp -fv kernel/arch/arm/boot/zImage $(rootfs)/zImage
	@cp -fv kernel/vmlinux $(rootfs)/vmlinux
	@find kernel -name "*.ko"|xargs -i cp -v {} $(rootfs)/example

extract-uboot:
	@tar -zxvf $(download)/u-boot-1.1.6_QQ2440.tgz -C $(CURDIR)
	@mv u-boot-1.1.6_QQ2440 u-boot

asuboot:u-boot
	@(cd u-boot;make QQ2440_config)
	@(cd u-boot;make all CROSS_COMPILE=$(gcc-common)/bin/arm-linux-)
	@(cd u-boot;cp -v u-boot $(rootfs))

extract-busybox:
	@tar -zxvf $(download)/busybox-1.2.0.tgz -C $(CURDIR)
	@mv busybox-1.2.0 busybox

busybox-menuconfig:
	@(cd busybox;make menuconfig)

busybox/.config:

asbusybox: busybox/.config
	@(cd busybox;make all CROSS_COMPILE=arm-linux-gnueabi-)
	@(cd busybox;make install CONFIG_PREFIX=$(rootfs))

extract-glibc:
	@bzip2 -dvk $(download)/glibc-2.22.tar.bz2
	@tar -xvf $(download)/glibc-2.22.tar -C $(CURDIR)
	@rm $(download)/glibc-2.22.tar
	@mv glibc-2.22 glibc

$(download)/glibc-2.22.tar.bz2:
	@(cd $(download;wget http://mirrors.ustc.edu.cn/gnu/libc/glibc-2.22.tar.bz2)
	@make extract-glibc

asglibc:$(download)/glibc-2.22.tar.bz2
	@(cd glibc;mkdir -pv build;	\
		cd build;	\
		../configure arm-linux-gnueabi --target=arm-linux-gnueabi --build=i686-pc-linux-gnu --prefix= --enable-add-ons;	\
		make;	\
		make install install_root=$(rootfs))

$(out)/sdcard.ext3:
	@dd if=/dev/zero of=$@ bs=1G count=2
	@sudo mkfs.ext3 $@

asrootfs:
	@cp aslinux/rootfs/* $(rootfs) -frv

sdcard:$(out)/sdcard.ext3 asrootfs
	@(cd $(out);mkdir -pv tmp;	\
		sudo mount -t ext3 $< tmp/ -o loop;	\
		sudo cp $(rootfs)/* tmp/ -rvf;	\
		sudo mkdir tmp/dev;	\
		sudo mknod tmp/dev/tty1 c 4 1;	\
		sudo mknod tmp/dev/tty2 c 4 2;	\
		sudo mknod tmp/dev/tty3 c 4 3;	\
		sudo mknod tmp/dev/tty4 c 4 4;	\
		sudo chmod +x tmp/etc/init.d/rcS;	\
		sudo umount tmp;	\
		rm tmp -fr)

example:
	@(cd example;make all)

clean:
	@(cd kernel;make clean)
	@(cd u-boot;make clean)
	@(cd busybox;make clean)
