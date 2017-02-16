#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2017  AS <parai@foxmail.com>
# *
# * This source code is free software; you can redistribute it and/or modify it
# * under the terms of the GNU General Public License version 2 as published by the
# * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
# *
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# * for more details.
# */

# default version of yocto want to be used
VERSION?=morty

rootfs?=$(CURDIR)/poky/build/tmp/deploy/images/qemux86

default:all

$(CURDIR)/poky:
	@(git clone -b $(VERSION) git://git.yoctoproject.org/poky.git)

$(CURDIR)/poky/openembedded-core:
	@(cd poky;git clone -b $(VERSION) git://git.openembedded.org/openembedded-core)

$(CURDIR)/poky/meta-intel-iot-security:
	(cd poky; git clone -b v1.0.0 https://github.com/01org/meta-intel-iot-security.git)

asyocto:$(CURDIR)/poky $(CURDIR)/poky/openembedded-core $(CURDIR)/poky/meta-intel-iot-security
	@echo " cd poky; source oe-init-build-env ; make asyoctosetup -C ../..; bitbake core-image-minimal "
	@echo " runqemu tmp/deploy/images/qemux86/core-image-minimal-qemux86.qemuboot.conf nographic"

asyoctosetup:
	@echo "BBLAYERS += \"`readlink -f poky/meta-intel-iot-security/meta-security-smack`\"" >> poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f poky/meta-intel-iot-security/meta-security-framework`\"" >> poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f poky/meta-as`\"" >> poky/build/conf/bblayers.conf
# check README of poky/meta-intel-iot-security/meta-security-smack: enable smack
	@echo "OVERRIDES .= \":smack\"" >> poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append = \" smack\"" >> poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append = \" pam\"" >> poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append += \" systemd\"" >> poky/build/conf/local.conf
	@echo "VIRTUAL-RUNTIME_init_manager = \"systemd\"" >> poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_BACKFILL_CONSIDERED = \"sysvinit\"" >> poky/build/conf/local.conf
	@echo "VIRTUAL-RUNTIME_initscripts = \"\"" >> poky/build/conf/local.conf
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"coreutils\"" >> poky/build/conf/local.conf
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"smack-userspace\"" >> poky/build/conf/local.conf
# install others
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"asrelease python cynara\"" >> poky/build/conf/local.conf

$(rootfs)/system.img:
	@dd if=/dev/zero of=$@ bs=1G count=2
#	@sudo mkfs.ext3 $@
	@echo " >> use fdisk to creat 2 primary partition, p1=+512M@2048, p2=left"
	@echo "$@Device Boot  Start         End      Blocks   Id  System"
	@echo "$@p1            2048     1050623      524288   83  Linux"
	@echo "$@p2         1050624     4194303     1571840   83  Linux"
	@fdisk $@
	@echo "  >> use parted to mkfs the 2 partition to ext2 filesyste"
	@parted $@

$(rootfs)/rootfs:
	@mkdir -p $@
	@tar xf $(rootfs)/core-image-minimal-qemux86.tar.bz2 -C $@

systemimg:$(rootfs)/system.img $(rootfs)/rootfs
	@(cd $(rootfs);mkdir -p tmp;	\
		parted system.img unit B print;	\
		sudo mount -o loop,offset=537919488 system.img tmp/; \
		pwd;sudo cp rootfs/* tmp/ -rf; \
		sudo umount tmp)

runqemu:
	sudo poky/build/tmp/sysroots/x86_64-linux/usr/bin/qemu-system-i386 -device virtio-net-pci,netdev=net0,mac=52:54:00:12:34:02 \
		-netdev tap,id=net0,ifname=tap0,script=no,downscript=no  -nographic  -cpu qemu32 -m 256 \
		-drive file=$(rootfs)/system.img,if=virtio,format=raw -vga vmware -show-cursor -usb -usbdevice tablet -device \
		virtio-rng-pci -kernel $(rootfs)/bzImage \
		-append 'root=/dev/vda2 rw highres=off  console=ttyS0 mem=256M ip=192.168.7.2::192.168.7.1:255.255.255.0 vga=0 uvesafb.mode_option=640x480-32 oprofile.timer=1 uvesafb.task_timeout=-1'

all:asyocto

