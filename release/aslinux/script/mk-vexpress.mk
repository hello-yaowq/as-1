# make file to build the platform for vexpress-a9
# start from a clean directory, 
# ln -fs /as/release/aslinux/script/mk-vexpress.mk makefile

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

out=$(CURDIR)/out
rootfs = $(out)/rootfs
download = $(CURDIR)/download

# first default make
all:$(rootfs) askernel asuboot asglibc asbusybox sdcard
	@echo "  >> build vexpress-a9 done <<"

$(rootfs):
	@mkdir -p $(rootfs)
	@mkdir -p $(rootfs)/lib/modules
	@mkdir -p $(rootfs)/lib/modules/3.18.0+
	@mkdir -p $(rootfs)/example

extract-kernel:
	@xz -vdk $(download)/linux-3.18.tar.xz
	@tar -xvf $(download)/linux-3.18.tar -C $(CURDIR)
	@rm $(download)/linux-3.18.tar
	@mv linux-3.18 kernel

$(download)/linux-3.18.tar.xz:
	@(cd $(download);wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.18.tar.xz)
	@make extract-kernel

kernel/.config:
	@(cd kernel;cp arch/arm/configs/vexpress_defconfig .config)
	@(cd kernel;make menuconfig O=.)

kernel-menuconfig:
	@(cd kernel;make menuconfig O=.)

askernel:$(download)/linux-3.18.tar.xz kernel/.config
#make uImage -j2 LOADADDR=0x60003000
	@(cd kernel;make all)
	@cp -fv kernel/arch/arm/boot/zImage $(rootfs)/zImage
	@cp -fv kernel/vmlinux $(rootfs)/vmlinux
	@find ./kernel -name "*.ko"|xargs -i cp -v {} $(rootfs)/example

u-boot:
	@git clone git://git.denx.de/u-boot.git

asuboot:u-boot
	@(cd u-boot;make vexpress_ca9x4_defconfig)
	@(cd u-boot;make all)
	@(cd u-boot;cp -v u-boot $(rootfs))

extract-busybox:
	@bzip2 -dvk $(download)/busybox-1.24.0.tar.bz2
	@tar -xvf $(download)/busybox-1.24.0.tar -C $(CURDIR)
	@rm $(download)/busybox-1.24.0.tar
	@mv busybox-1.24.0 busybox

$(download)/busybox-1.24.0.tar.bz2:
	@(cd $(download);wget http://busybox.net/downloads/busybox-1.24.0.tar.bz2)
	@make extract-busybox

busybox-menuconfig:
	@(cd busybox;make menuconfig)

busybox/.config:busybox-menuconfig

busybox:$(download)/busybox-1.24.0.tar.bz2 busybox/.config
	@(cd busybox;make all)
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
	@(cd glibc/build;make clean)
	@(cd example;make clean)
