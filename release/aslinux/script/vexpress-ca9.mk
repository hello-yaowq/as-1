# make file to build the platform for vexpress-ca9
# start from a clean directory, 
# ln -fs /as/release/aslinux/script/vexpress-ca9.mk makefile

#   As in China that often some URL resoure especial GOOGLE is not accessable, so a mirror is an alternate 
# solution, such as https://mirrors.tuna.tsinghua.edu.cn/

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

out=$(CURDIR)/out
rootfs = $(out)/rootfs
download = $(CURDIR)/download

SUPERFN = python $(CURDIR)/../../make/superfn.py

# first default make
#all:$(rootfs) askernel asuboot asglibc asbusybox astslib asqt sdcard
all:$(rootfs) askernel asglibc asbusybox asamb ascanutil sdcard
	@echo "  >> build vexpress-a9 done <<"

# 4.8.6 or 5.5.1
qt-version?=4.8.6

$(rootfs):
	@mkdir -p $(rootfs)
	@mkdir -p $(rootfs)/lib/modules
	@mkdir -p $(rootfs)/lib/modules/3.18.0+
	@mkdir -p $(rootfs)/example
	@mkdir -p $(download)

$(download)/jre-6u45-linux-i586.bin:
	@(cd $(download);wget http://download.oracle.com/otn/java/jdk/6u45-b06/jre-6u45-linux-i586.bin;  \
		wget http://download.oracle.com/otn/java/jdk/6u43-b01/jdk-6u43-linux-i586.bin)

asjava:$(download)/jre-6u45-linux-i586.bin

$(download)/qemu_v2.0.0.tar.gz:
	@(cd $(download);wget https://github.com/qemu/qemu/archive/v2.0.0.tar.gz -O qemu_v2.0.0.tar.gz)

$(download)/qemu:$(download)/qemu_v2.0.0.tar.gz
	@(tar xf $(download)/qemu_v2.0.0.tar.gz -C .)

asqemu:$(download)/qemu

$(download)/sqlite-amalgamation-3.5.6.tar.gz:
	@(cd $(download);wget http://www.sqlite.org/sqlite-amalgamation-3.5.6.tar.gz)

sqlite-3.5.6:$(download)/sqlite-amalgamation-3.5.6.tar.gz
	@tar zxf $(download)/sqlite-amalgamation-3.5.6.tar.gz -C .

assqlite:sqlite-3.5.6
	@(cd sqlite-3.5.6;./configure --host=$(ARCH) CC=$(CROSS_COMPILE)gcc --prefix=$(rootfs)/usr  \
		 --enable-shared --disable-readline --disable-dynamic-extensions;  	\
		make; make install)

$(download)/Python-2.5.1.tar.bz2:
	@(cd $(download);wget http://www.python.org/ftp/python/2.5.1/Python-2.5.1.tar.bz2)

Python-2.5.1:$(download)/Python-2.5.1.tar.bz2
	@(tar jxf $(download)/Python-2.5.1.tar.bz2 -C .)

aspython:Python-2.5.1
	@(cd Python-2.5.1;mkdir -p build.pc;cd build.pc;SVNVERSION="Unversioned directory" ../configure CC=gcc --host=x86 ;make all;)
# modify the configure to disable the %zd printf format check
# For some PC, when do configure, should add 'SVNVERSION="Unversioned directory"', don't ask me why
	@(cd Python-2.5.1;mkdir build.arm;cd build.arm;  \
		sed -i "22092c if 0; then" ../configure;	\
		sed -i "22168c fi" ../configure;	\
		SVNVERSION="Unversioned directory" ../configure --prefix=$(rootfs)/usr --disable-ipv6 --host=$(ARCH) CC=$(CROSS_COMPILE)gcc --enable-shared;  \
		$(SUPERFN) replace.file.all Makefile './\x24(BUILDPYTHON)' ../build.pc/python;  \
		make all;make install)

can-utils:
	@git clone  https://github.com/linux-can/can-utils.git

ascanutil: can-utils
	@(cd can-utils;./autogen.sh;	\
		./configure --host=$(ARCH) --prefix=$(rootfs)/usr CC=$(CROSS_COMPILE)gcc;	\
		make clean;make all;make install)

libsocketcan:
	@git clone git://git.pengutronix.de/git/tools/libsocketcan.git
	@(cd libsocketcan;git checkout v0.0.10)

canutils:
	@git clone git://git.pengutronix.de/tools/canutils
	@(cd canutils;git checkout canutils-4.0.6)

ascanutils: libsocketcan canutils 
	(cd libsocketcan;./autogen.sh;	\
		./configure --host=$(ARCH) CC=$(CROSS_COMPILE)gcc;	\
		make clean;make all)
	(cd canutils;./autogen.sh;	\
		sed -i "12522c pkg_failed=no" configure;	\
		./configure --host=$(ARCH) --prefix=$(rootfs)/usr CC=$(CROSS_COMPILE)gcc CFLAGS=-I$(CURDIR)/libsocketcan/include LDFLAGS="-lsocketcan -L$(CURDIR)/libsocketcan/src/.libs";	\
		make clean;make all;make install)

automotive-message-broker:
	@git clone https://github.com/otcshare/automotive-message-broker.git
	@(cd automotive-message-broker;git checkout 0.14;mkdir build)

asamb:automotive-message-broker
	@(cd automotive-message-broker/build; cmake ..; make)

patch-kernel:
	@(cd ../kernel/drivers/remoteproc/rproc-asvirt; make dep)
	@(cp ../kernel . -rvf)
	@(cd kernel; patch -p1 < aspatch/0001-aslinux-add-virtual-pinctrl-and-rpmsg-driver.patch)
	@(cd kernel; patch -p1 < aspatch/0002-aslinux-add-virtual-CAN-driver-based-on-RPMSG.patch)

extract-kernel:
	@xz -dk $(download)/linux-3.18.tar.xz
	@tar -xf $(download)/linux-3.18.tar -C $(CURDIR)
	@rm $(download)/linux-3.18.tar
	@mv linux-3.18 kernel
	@make patch-kernel

$(download)/linux-3.18.tar.xz:
	@(cd $(download);wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.18.tar.xz)
	@make extract-kernel

kernel/.config:
#	@(cd kernel;cp arch/arm/configs/vexpress_defconfig .config)
#	@(cd kernel;make menuconfig O=.)
	@(cd kernel;cp aspatch/vexpress_defconfig .config)

kernel-menuconfig:
	@(cd kernel;make menuconfig O=.)

askernel:$(rootfs) $(download)/linux-3.18.tar.xz kernel/.config
#make uImage -j2 LOADADDR=0x60003000
	@(cd kernel;make all)
	@cp -f kernel/arch/arm/boot/zImage $(rootfs)/zImage
	@cp -f kernel/vmlinux $(rootfs)/vmlinux
	@find kernel -name "*.ko"|xargs -i cp -v {} $(rootfs)/example
	@cp -f kernel/arch/arm/boot/dts/vexpress-v2p-ca9.dtb $(rootfs)

u-boot:
	@git clone git://git.denx.de/u-boot.git

asuboot:u-boot
	@(cd u-boot;make vexpress_ca9x4_defconfig)
	@(cd u-boot;make all)
	@(cd u-boot;cp -v u-boot $(rootfs))

extract-busybox:
	@bzip2 -dk $(download)/busybox-1.24.0.tar.bz2
	@tar -xf $(download)/busybox-1.24.0.tar -C $(CURDIR)
	@rm $(download)/busybox-1.24.0.tar
	@mv busybox-1.24.0 busybox

$(download)/busybox-1.24.0.tar.bz2:
	@(cd $(download);wget http://busybox.net/downloads/busybox-1.24.0.tar.bz2)
	@make extract-busybox

busybox-menuconfig:
	@(cd busybox;make menuconfig)

busybox/.config:
	#@(cd busybox;make menuconfig)
	@(cd busybox;cp ../kernel/aspatch/busybox_defconfig .config)

asbusybox:$(download)/busybox-1.24.0.tar.bz2 busybox/.config
	@(cd busybox;make all)
	@(cd busybox;make install CONFIG_PREFIX=$(rootfs))

extract-glibc:
	@bzip2 -dk $(download)/glibc-2.22.tar.bz2
	@tar -xf $(download)/glibc-2.22.tar -C $(CURDIR)
	@rm $(download)/glibc-2.22.tar
	@mv glibc-2.22 glibc

$(download)/glibc-2.22.tar.bz2:
	@(cd $(download);wget http://mirrors.ustc.edu.cn/gnu/libc/glibc-2.22.tar.bz2)
	@make extract-glibc

asglibc:$(download)/glibc-2.22.tar.bz2
	@(cd glibc;mkdir -pv build;	\
		cd build;	\
		../configure arm-linux-gnueabi --target=arm-linux-gnueabi --build=i686-pc-linux-gnu --prefix= --enable-add-ons;	\
		make;	\
		make install install_root=$(rootfs))

$(download)/qt-everywhere-opensource-src-5.5.1.tar.gz:$(rootfs)
	@(cd $(download);wget http://101.44.1.124/files/A165000004244025/anychimirror101.mirrors.tds.net/pub/Qt/archive/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.gz)
	@make extract-qt

extract-qt-5.5.1:$(download)/qt-everywhere-opensource-src-5.5.1.tar.gz
	@(gunzip -k $(download)/qt-everywhere-opensource-src-5.5.1.tar.gz)
	@(tar -xf$(download)/qt-everywhere-opensource-src-5.5.1.tar -C $(CURDIR))
	@rm $(download)/qt-everywhere-opensource-src-5.5.1.tar

extract-qt-4.8.6:$(download)/qt-everywhere-opensource-src-4.8.6.tar.gz
	@(gunzip -k $(download)/qt-everywhere-opensource-src-4.8.6.tar.gz)
	@(tar -xf$(download)/qt-everywhere-opensource-src-4.8.6.tar -C $(CURDIR))
	@rm $(download)/qt-everywhere-opensource-src-4.8.6.tar

$(download)/qt-everywhere-opensource-src-4.8.6.tar.gz:
	@(cd $(download);wget http://101.44.1.117/files/82540000020A32A4/mirrors.ustc.edu.cn/qtproject/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz)
	@make extract-qt-4.8.6

asqt-4.8.6:
	@(cd qt-everywhere-opensource-src-$(qt-version);	\
		./configure \
			-opensource \
			-confirm-license \
			-release -shared \
			-embedded arm \
			-xplatform qws/linux-arm-g++ \
			-depths 16,18,24 \
			-fast \
			-optimized-qmake \
			-pch \
			-qt-sql-sqlite \
			-qt-libjpeg \
			-qt-zlib \
			-qt-libpng \
			-qt-freetype \
			-little-endian -host-little-endian \
			-no-qt3support \
			-no-libtiff -no-libmng \
			-no-opengl \
			-no-mmx -no-sse -no-sse2 \
			-no-3dnow \
			-no-openssl \
			-no-webkit \
			-no-qvfb \
			-no-phonon \
			-no-nis \
			-no-opengl \
			-no-cups \
			-no-glib \
			-no-xcursor -no-xfixes -no-xrandr -no-xrender \
			-no-separate-debug-info \
			-nomake examples -nomake tools -nomake docs \
			-qt-mouse-tslib \
			-I$(rootfs)/include \
			-L$(rootfs)/usr/lib;	\
		make;	\
		make install INSTALL_ROOT=$(rootfs)/qt)

asqt-5.5.1:
	@(echo "  >> I don't know how to build"

asqt: $(download)/qt-everywhere-opensource-src-$(qt-version).tar.gz asqt-$(qt-version)
	@(cd qt-everywhere-opensource-src-$(qt-version);./configure -embedded arm;make;make install INSTALL_ROOT=$(rootfs)/qt)

tslib:
	@git clone https://github.com/kergoth/tslib.git
	@(cd tslib;git checkout 1.1 -f;git st|xargs -i rm {} -fvr)

# something wrong, do manual build step by step
astslib:tslib
	@(cd tslib;./autogen.sh;	\
		echo "av_cv_func_malloc_0_nonnull=yes" > arm-linux-gnueabi.cache;	\
		./configure --host=arm-linux-gnueabi --cache-file=arm-linux-gnueabi.cache \
		-prefix=$(CURDIR)/tslib/install)
	@(cd tslib;make install)
	@(cd tslib;cp -frv install/* $(rootfs))

$(out)/sdcard.ext3:
	@dd if=/dev/zero of=$@ bs=1G count=2
	@sudo mkfs.ext3 $@

asrootfs:

sdcard:$(out)/sdcard.ext3 asrootfs
	@(cd $(out);mkdir -p tmp;	\
		sudo mount -t ext3 sdcard.ext3 tmp/ -o loop;	\
		sudo cp $(rootfs)/* tmp/ -rf;	\
		sudo mkdir tmp/dev;	\
		sudo mknod tmp/dev/tty1 c 4 1;	\
		sudo mknod tmp/dev/tty2 c 4 2;	\
		sudo mknod tmp/dev/tty3 c 4 3;	\
		sudo mknod tmp/dev/tty4 c 4 4;	\
		sudo mkdir tmp/proc tmp/tmp tmp/sys;	\
		sudo cp ../../rootfs/* tmp/ -rvf;	\
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
	@(cd tslib;make clean;./autogen-clean.sh)
	@(cd can-utils;make clean)
	@(cd canutils;make clean)
	@(cd libsocketcan;make clean)
