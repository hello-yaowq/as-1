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
# "agl" or "poky"
POKY?=agl

# valid: x86, arm, arm64
export ARCH?=arm64

export MACHINE?=qemu${ARCH}

rootfs?=$(CURDIR)/$(POKY)/poky/build/tmp/deploy/images/${MACHINE}

ifeq (${ARCH},x86)
export  CROSS_COMPILE?=$(CURDIR)/$(POKY)/poky/build/tmp/sysroots/x86_64-linux/usr/bin/i586-poky-linux/i586-poky-linux-
endif

ifeq (${ARCH},arm)
export  CROSS_COMPILE?=$(CURDIR)/$(POKY)/poky/build/tmp/sysroots/x86_64-linux/usr/bin/arm-poky-linux-guneabi/arm-poky-linux-gnueabi-
endif

ifeq (${ARCH},arm64)
export  CROSS_COMPILE?=$(CURDIR)/$(POKY)/poky/build/tmp/sysroots/x86_64-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-
endif

default:all

# ----------------------------- [ issues ] -----------------------------------------------------
# FIXME: for build of security-manager, there is some error, fix them manually
#  1) '#include <vector>' && 'using namesapce std;'
#  2) '#define XATTR_NAME_SMACKIPIN "SMACK64IPIN"' && '#define XATTR_NAME_SMACKIPOUT "SMACK64IPOUT"'
#  3) '#include_next <stdlib.h>' --> '#include "./stdlib.h"' ... etc

# ----------------------------- [ AGL ] -----------------------------------------------------
# https://mirrors.tuna.tsinghua.edu.cn/help/git-repo/
$(HOME)/bin/repo:
	@mkdir -p $(HOME)/bin
	@curl https://mirrors.tuna.tsinghua.edu.cn/git/git-repo -o $@
	@chmod +x $@

# get AGL for a refference for the study of yocto
$(CURDIR)/agl:$(HOME)/bin/repo
	@mkdir -p $@
	@(cd agl;export REPO_URL='https://mirrors.tuna.tsinghua.edu.cn/git/git-repo/';  \
		repo init -b $(VERSION) -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo; repo sync)

# ----------------------------- [ yocto ] -----------------------------------------------------
$(CURDIR)/$(POKY):
	@mkdir -p $@

$(CURDIR)/$(POKY)/poky:
	@(cd $(POKY);git clone -b $(VERSION) git://git.yoctoproject.org/poky.git)

$(CURDIR)/$(POKY)/openembedded-core:
	@(cd $(POKY);git clone -b $(VERSION) git://git.openembedded.org/openembedded-core)

$(CURDIR)/$(POKY)/meta-openembedded:
	@(cd $(POKY);git clone -b $(VERSION) git://git.openembedded.org/meta-openembedded)

$(CURDIR)/$(POKY)/meta-intel-iot-security:
	(cd $(POKY); git clone -b v1.0.0 https://github.com/01org/meta-intel-iot-security.git)

$(CURDIR)/$(POKY)/meta-amb:
	@(cd $(POKY); git clone https://github.com/tripzero/meta-amb.git)

# remoce the cmake of meta-qt5 as the version is too old.
$(CURDIR)/$(POKY)/meta-qt5:
	@(cd $(POKY); git clone -b $(VERSION) https://github.com/meta-qt5/meta-qt5.git)

# ----------------------------- [ aslinux build ] -----------------------------------------------------
as$(POKY):$(CURDIR)/$(POKY) $(CURDIR)/$(POKY)/meta-intel-iot-security $(CURDIR)/$(POKY)/meta-openembedded \
		$(CURDIR)/$(POKY)/meta-amb $(CURDIR)/$(POKY)/meta-qt5
	@echo " cd $(POKY)/poky; source oe-init-build-env ; make as$(POKY)setup -C $(CURDIR); bitbake core-image-minimal "
	@echo " runqemu tmp/deploy/images/${MACHINE}/core-image-minimal-${MACHINE}.qemuboot.conf nographic"

as$(POKY)setup:
	@echo "MACHINE ?= \"${MACHINE}\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "BBLAYERS += \"`readlink -f $(POKY)/meta-intel-iot-security/meta-security-smack`\"" >> $(POKY)/poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f $(POKY)/meta-intel-iot-security/meta-security-framework`\"" >> $(POKY)/poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f $(POKY)/meta-openembedded/meta-oe`\"" >> $(POKY)/poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f $(POKY)/meta-qt5`\"" >> $(POKY)/poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f $(POKY)/meta-amb`\"" >> $(POKY)/poky/build/conf/bblayers.conf
	@echo "BBLAYERS += \"`readlink -f meta-as`\"" >> $(POKY)/poky/build/conf/bblayers.conf
# check README of $(POKY)/meta-intel-iot-security/meta-security-smack: enable smack
	@echo "OVERRIDES .= \":smack\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append = \" smack\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append = \" pam\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_append += \" systemd\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "VIRTUAL-RUNTIME_init_manager = \"systemd\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "DISTRO_FEATURES_BACKFILL_CONSIDERED = \"sysvinit\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "VIRTUAL-RUNTIME_initscripts = \"\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"coreutils\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"smack-userspace\"" >> $(POKY)/poky/build/conf/local.conf
# install others
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"asrelease python cynara cryptsetup qtbase security-manager\"" >> $(POKY)/poky/build/conf/local.conf
	@echo "CORE_IMAGE_EXTRA_INSTALL += \"strace e2fsprogs attr openssh\"" >> $(POKY)/poky/build/conf/local.conf

$(rootfs)/system.img:
	@dd if=/dev/zero of=$@ bs=1G count=2
	@sudo mkfs.ext4 $@

# will be /dev/vdb
$(rootfs)/userdata.img:
	@dd if=/dev/zero of=$@ bs=1G count=1
	@sudo mkfs.ext4 $@

$(rootfs)/rootfs:
	@mkdir -p $@
	@tar xf $(rootfs)/core-image-minimal-${MACHINE}.tar.bz2 -C $@

systemimg:$(rootfs)/system.img $(rootfs)/userdata.img $(rootfs)/rootfs
	@(cd $(rootfs);mkdir -p tmp;	\
		parted system.img unit B print;	\
		sudo mount -t ext4 -o loop system.img tmp/; \
		pwd;sudo cp rootfs/* tmp/ -rf; \
		sudo umount tmp)

$(CURDIR)/.qemux86.qemuboot.conf:
	@echo "[config_bsp]" > $@
	@echo "machine = ${MACHINE}" >> $@
	@echo "tune_arch = i586" >> $@
	@echo "deploy_dir_image = $(POKY)/poky/build/tmp/deploy/images/${MACHINE}" >> $@
	@echo "kernel_imagetype = .bzImage" >> $@
	@echo "image_name = system" >> $@
	@echo "image_link_name = .system.img" >> $@
	@echo "staging_dir_native = $(POKY)/poky/build/tmp/sysroots/x86_64-linux" >> $@
	@echo "staging_bindir_native = $(POKY)/poky/build/tmp/sysroots/x86_64-linux/usr/bin" >> $@
	@echo "staging_dir_host = $(POKY)/poky/build/tmp/sysroots/${MACHINE}" >> $@
	@echo "qb_cpu_kvm_x86-64 = -cpu kvm64" >> $@
	@echo "qb_mem = -m 256" >> $@
	@echo "qb_kernel_cmdline_append = vga=0 uvesafb.mode_option=640x480-32 oprofile.timer=1 uvesafb.task_timeout=-1" >> $@
	@echo "qb_cpu_kvm_x86 = -cpu kvm32" >> $@
	@echo "qb_system_name_x86 = qemu-system-i386" >> $@
	@echo "qb_audio_opt = -soundhw ac97,es1370" >> $@
	@echo "qb_default_kernel = .bzImage.bin" >> $@
	@echo "qb_audio_drv = alsa" >> $@
	@echo "qb_cpu_x86 = -cpu qemu32" >> $@
	@echo "qb_serial_opt = -serial mon:stdio -serial null" >> $@
	@echo "qb_opt_append = -vga vmware -show-cursor -usb -usbdevice tablet -device virtio-rng-pci -drive file=$(rootfs)/userdata.img,if=virtio,format=raw" >> $@
	@echo "qb_cpu_x86-64 = -cpu core2duo" >> $@
	@echo "qb_default_fstype = ext4" >> $@
	@echo "qb_system_name_x86-64 = qemu-system-x86_64" >> $@
	@echo "qb_system_name = qemu-system-i386" >> $@
	@echo "qb_cpu_kvm = -cpu kvm32" >> $@
	@echo "qb_cpu = -cpu qemu32" >> $@

$(CURDIR)/.qemuarm.qemuboot.conf:
	@echo "[config_bsp]" > $@
	@echo "machine = qemuarm" >> $@
	@echo "tune_arch = arm" >> $@
	@echo "deploy_dir_image = ${POKY}/poky/build/tmp/deploy/images/qemuarm" >> $@
	@echo "kernel_imagetype = .zImage" >> $@
	@echo "image_name = system" >> $@
	@echo "image_link_name = .system.img" >> $@
	@echo "staging_dir_native = ${POKY}/poky/build/tmp/sysroots/x86_64-linux" >> $@
	@echo "staging_bindir_native = ${POKY}//poky/build/tmp/sysroots/x86_64-linux/usr/bin" >> $@
	@echo "staging_dir_host = ${POKY}/poky/build/tmp/sysroots/qemuarm" >> $@
	@echo "qb_opt_append = -show-cursor -usb -usbdevice tablet -device virtio-rng-pci" >> $@
	@echo "qb_dtb = .zImage-versatile-pb.dtb" >> $@
	@echo "qb_default_fstype = ext4" >> $@
	@echo "qb_mem = -m 256" >> $@
	@echo "qb_serial_opt = -serial mon:stdio -serial null" >> $@
	@echo "qb_system_name = qemu-system-arm" >> $@
	@echo "qb_machine = -machine versatilepb" >> $@
	@echo "qb_kernel_cmdline_append = console=ttyAMA0,115200 console=tty" >> $@
	@echo "qb_default_kernel = .zImage.bin" >> $@

$(CURDIR)/.qemuarm64.qemuboot.conf:
	@echo "[config_bsp]" > $@
	@echo "machine = qemuarm64" >> $@
	@echo "tune_arch = aarch64" >> $@
	@echo "deploy_dir_image = ${POKY}/poky/build/tmp/deploy/images/qemuarm64" >> $@
	@echo "kernel_imagetype = .Image" >> $@
	@echo "image_name = system" >> $@
	@echo "image_link_name = .system.img" >> $@
	@echo "staging_dir_native = ${POKY}/poky/build/tmp/sysroots/x86_64-linux" >> $@
	@echo "staging_bindir_native = ${POKY}/poky/build/tmp/sysroots/x86_64-linux/usr/bin" >> $@
	@echo "staging_dir_host = ${POKY}/poky/build/tmp/sysroots/qemuarm64" >> $@
	@echo "qb_mem = -m 512" >> $@
	@echo "qb_default_fstype = ext4" >> $@
	@echo "qb_cpu = -cpu cortex-a57" >> $@
	@echo "qb_serial_opt = -device virtio-serial-device -chardev null,id=virtcon -device virtconsole,chardev=virtcon" >> $@
	@echo "qb_tcpserial_opt =  -device virtio-serial-device -chardev socket,id=virtcon,port=@PORT@,host=127.0.0.1 -device virtconsole,chardev=virtcon" >> $@
	@echo "qb_slirp_opt = -netdev user,id=net0 -device virtio-net-device,netdev=net0" >> $@
	@echo "qb_kernel_cmdline_append = console=ttyAMA0,38400" >> $@
	@echo "qb_opt_append = -show-cursor -device virtio-rng-pci -monitor null" >> $@
	@echo "qb_machine = -machine virt" >> $@
	@echo "qb_system_name = qemu-system-aarch64" >> $@
	@echo "qb_default_kernel = .Image.bin" >> $@
	@echo "qb_rootfs_opt = -drive id=disk0,file=@ROOTFS@,if=none,format=raw -device virtio-blk-device,drive=disk0" >> $@
	@echo "qb_tap_opt = -netdev tap,id=net0,ifname=@TAP@,script=no,downscript=no -device virtio-net-device,netdev=net0,mac=@MAC@" >> $@

$(CURDIR)/.system.img.ext4:
	@ln -fs $(rootfs)/system.img $@

# for example do debug kernel, do: export qemuparams="-s -S"
export qemuparams?="-nographic"
runqemu:$(CURDIR)/.${MACHINE}.qemuboot.conf $(CURDIR)/.system.img.ext4
	@(export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$(POKY)/poky/scripts/;$(POKY)/poky/scripts/runqemu $< qemuparams="$(qemuparams)")

kernel-menuconfig:
	@(cd kernel4.8;make menuconfig O=.)

kernel4.8/.config:
	@(cp $(POKY)/poky/build/tmp/work-shared/${MACHINE}/kernel-build-artifacts/.config $@ -v)

$(CURDIR)/kernel4.8:
	@(cp $(POKY)/poky/build/tmp/work-shared/${MACHINE}/kernel-source $@ -r)

# this is for the purpose to fast build the kernel and debug
kernel:$(CURDIR)/kernel4.8 kernel4.8/.config
	@(cd kernel4.8;make)
ifeq (${ARCH},x86)
	@(ln -fs kernel4.8/arch/i386/boot/bzImage .bzImage)
endif
ifeq (${ARCH},arm)
	@(ln -fs kernel4.8/arch/${ARCH}/boot/zImage .zImage)
	@(ln -fs kernel4.8//arch/${ARCH}/boot/dts/versatile-pb.dtb .zImage-versatile-pb.dtb)
endif
ifeq (${ARCH},arm64)
	@(ln -fs kernel4.8/arch/${ARCH}/boot/Image .Image)
endif

all:as$(POKY)

