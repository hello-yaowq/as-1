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

if [ ! -d qemu ] ; then
  git clone git://repo.or.cz/qemu/mini2440.git qemu
fi
cd qemu
./configure --target-list=arm-softmmu 
make -j4 
cd ..

if [ ! -d kernel ] ; then
  wget http://repo.or.cz/w/linux-2.6/mini2440.git/snapshot/HEAD.tar.gz
  tar xzvf HEAD.tar.gz
fi


if [ ! -d uboot ] ; then
  git clone  git://repo.or.cz/w/u-boot-openmoko/mini2440.git  uboot
fi
cd uboot
make mini2440_config 
make -j4
cd .. 
