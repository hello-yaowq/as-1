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

all:asyocto

