# make file to build the platform for vexpress-ca9
# start from a clean directory, 
# ln -fs /as/release/aslinux/script/vexpress-ca9.mk makefile

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

out=$(CURDIR)/out
rootfs = $(out)/rootfs
download = $(CURDIR)/download

# first default make
#all:$(rootfs) askernel asuboot asglibc asbusybox astslib asqt sdcard
all:$(rootfs) askernel asglibc asbusybox sdcard
	@echo "  >> build vexpress-a9 done <<"

# 4.8.6 or 5.5.1
qt-version?=4.8.6

$(rootfs):
	@mkdir -p $(rootfs)
	@mkdir -p $(rootfs)/lib/modules
	@mkdir -p $(rootfs)/lib/modules/3.18.0+
	@mkdir -p $(rootfs)/example
	@mkdir -p $(download)

patch-kernel:
	@(cd ../kernel/drivers/remoteproc/rproc-asvirt; make dep)
	@(cp ../kernel . -rvf)
	@(cd kernel; patch -p1 < aspatch/0001-aslinux-add-virtual-pinctrl-and-rpmsg-driver.patch)
	

extract-kernel:
	@xz -vdk $(download)/linux-3.18.tar.xz
	@tar -xvf $(download)/linux-3.18.tar -C $(CURDIR)
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

askernel:$(download)/linux-3.18.tar.xz kernel/.config
#make uImage -j2 LOADADDR=0x60003000
	@(cd kernel;make all)
	@cp -fv kernel/arch/arm/boot/zImage $(rootfs)/zImage
	@cp -fv kernel/vmlinux $(rootfs)/vmlinux
	@find kernel -name "*.ko"|xargs -i cp -v {} $(rootfs)/example
	@cp -fv kernel/arch/arm/boot/dts/vexpress-v2p-ca9.dtb $(rootfs)

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

busybox/.config:
	@(cd busybox;make menuconfig)

asbusybox:$(download)/busybox-1.24.0.tar.bz2 busybox/.config
	@(cd busybox;make all)
	@(cd busybox;make install CONFIG_PREFIX=$(rootfs))

extract-glibc:
	@bzip2 -dvk $(download)/glibc-2.22.tar.bz2
	@tar -xvf $(download)/glibc-2.22.tar -C $(CURDIR)
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

$(download)/qt-everywhere-opensource-src-5.5.1.tar.gz:
	@(cd $(download);wget http://101.44.1.124/files/A165000004244025/anychimirror101.mirrors.tds.net/pub/Qt/archive/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.gz)
	@make extract-qt

extract-qt-5.5.1:$(download)/qt-everywhere-opensource-src-5.5.1.tar.gz
	@(gunzip -kv $(download)/qt-everywhere-opensource-src-5.5.1.tar.gz)
	@(tar -xvf$(download)/qt-everywhere-opensource-src-5.5.1.tar -C $(CURDIR))
	@rm $(download)/qt-everywhere-opensource-src-5.5.1.tar

extract-qt-4.8.6:$(download)/qt-everywhere-opensource-src-4.8.6.tar.gz
	@(gunzip -kv $(download)/qt-everywhere-opensource-src-4.8.6.tar.gz)
	@(tar -xvf$(download)/qt-everywhere-opensource-src-4.8.6.tar -C $(CURDIR))
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
	@(cd $(out);mkdir -pv tmp;	\
		sudo mount -t ext3 sdcard.ext3 tmp/ -o loop;	\
		sudo cp $(rootfs)/* tmp/ -rvf;	\
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
