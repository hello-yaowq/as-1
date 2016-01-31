#!/bin/bash

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

if [ ! -d out ] ; then
	mkdir out
fi

cd out
if [ ! -d rootfs ] ; then
	mkdir rootfs
fi
cd ..


if [ ! -d linux-3.18 ] ; then
  if [ ! -f linux-3.18.tar.xz ] ; then
    wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.18.tar.xz
  fi
  xz -vdk linux-3.18.tar.xz
  tar -xvf linux-3.18.tar
  rm linux-3.18.tar
fi
cd linux-3.18
if [ ! -f .config ] ; then
  cp arch/arm/configs/vexpress_defconfig .config
  make menuconfig O=.
fi
make uImage -j2 LOADADDR=0x60003000
cd ..
cp -fv linux-3.18/arch/arm/boot/zImage out/zImage
cp -fv linux-3.18/arch/arm/boot/zImage out/rootfs/zImage
cp -fv linux-3.18/vmlinux out/rootfs/vmlinux

if [ ! -d u-boot ] ; then
  git clone git://git.denx.de/u-boot.git
fi
cd u-boot
make vexpress_ca9x4_defconfig
make all
cp u-boot ../out/ -v
cd ..

if [ ! -d busybox-1.24.0 ] ; then
  if [ ! -f busybox-1.24.0.tar.bz2 ] ; then
    wget http://busybox.net/downloads/busybox-1.24.0.tar.bz2
  fi
  bzip2 -dvk busybox-1.24.0.tar.bz2
  tar -xvf busybox-1.24.0.tar
  rm busybox-1.24.0.tar
fi

cd busybox-1.24.0
if [ ! -f .config ] ; then
    make menuconfig
fi
make all
make install CONFIG_PREFIX=../out/rootfs
cd ..

if [ ! -d glibc-2.22 ] ; then
  if [ ! -f glibc-2.22.tar.bz2 ] ; then
    wget http://mirrors.ustc.edu.cn/gnu/libc/glibc-2.22.tar.bz2
  fi
  bzip2 -dvk glibc-2.22.tar.bz2
  tar -xvf glibc-2.22.tar
fi
cd glibc-2.22
mkdir -pv build
cd build
../configure arm-linux-gnueabi --target=arm-linux-gnueabi --build=i686-pc-linux-gnu --prefix= --enable-add-ons
make
make install install_root=../../out/rootfs 
cd ..
cd ..

cd out
if [ ! -f sdcard.ext3 ] ; then
	dd if=/dev/zero of=sdcard.ext3 bs=1G count=2
	sudo mkfs.ext3 sdcard.ext3
fi
mkdir -pv tmp
sudo mount -t ext3 sdcard.ext3 tmp/ -o loop

sudo cp rootfs/* tmp/ -rvf

sudo umount tmp
rm tmp -fr

