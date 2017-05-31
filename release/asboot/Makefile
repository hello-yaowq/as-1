#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2015  AS <parai@foxmail.com>
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
target-y = $(board)

obj-dir = $(CURDIR)/obj/$(board)-$(compiler)
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src/$(board)-$(compiler)
inc-dir = $(CURDIR)/src/$(board)-$(compiler)/include

prj-dir = $(subst /release/asboot,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
LUA   = $(prj-dir)/com/as.tool/lua
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system
# --------------------------------- [ GENERAL SPECIFICATION ] ------------------------------------------- #		 
debug ?= true
ifeq ($(host), Linux)
PY27 = python2
PY34 = python3
else
PY27 = C:Python27/python
PY34 = C:Python34/python
HOME ?= C:/Users/parai.wang
ifeq ($(HOME), $(wildcard $(HOME)))
#else
#$(error fix your home path)
endif
endif
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
DEPC  = $(PY27) $(prj-dir)/release/make/dep.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY34) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py

use_nvm?=no
led_app?=yes

-include script/$(board).mk

inc-y += -I$(inc-dir)
def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board
def-y += -D__AS_BOOTLOADER__

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

BSW:	
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) true)

OS:
ifeq ($(board),stm32f107vc-flsdrv)
else
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) false)	
endif

studio:
	@(cd $(src-dir)/config.infrastructure.system;$(STUDIO) $(src-dir))	

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) OS

all:exe

depc:
	@($(DEPC) $(obj-dir) $(obj-dir)/as.dep)

clean: 
	@rm -frv ./obj/*

# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=cortexm3-gcc
12:
	make all board=stm32f107vc compiler=cortexm3-gcc debug=true
13:
	make all board=stm32f107vc compiler=cortexm3-gcc debug=false	

15:
	make dep board=stm32f107vc compiler=cortexm3-icc
16:
	make all board=stm32f107vc compiler=cortexm3-icc debug=true
17:
	make all board=stm32f107vc compiler=cortexm3-icc debug=false

# ---------------------------------- [ STM32F107VC-FLSDRV ] ---------------------------------- #
21:
	make dep board=stm32f107vc-flsdrv compiler=cortexm3-gcc
22:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-gcc debug=true
23:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-gcc debug=false	

25:
	make dep board=stm32f107vc-flsdrv compiler=cortexm3-icc
26:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-icc debug=true
27:
	make all board=stm32f107vc-flsdrv compiler=cortexm3-icc debug=false

# ---------------------------------- [ qemu-rpi bcm2835 ] ----------------------------- #
41:
	make dep board=bcm2835 compiler=gcc
42:
	make all board=bcm2835 compiler=gcc

$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz:
	@wget https://github.com/idrawone/qemu-rpi/raw/master/tools/qemu-system-arm-rpi_UART1.tar.gz
	@tar xf $@

qemuparams?="-nographic"
qemu-rpi:$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz
#	@(./qemu-system-arm -cpu arm1176 -m 128 -M raspi ${qemuparams} -kernel out/bcm2835.exe -s -S)
	@(./qemu-system-arm -cpu arm1176 -m 128 -M raspi ${qemuparams} -kernel out/bcm2835.exe -serial tcp:127.0.0.1:1103,server)

# ---------------------------------- [ s3c2440a    ] ---------------------------------- #
51:
	make dep board=s3c2440a compiler=arm920t-gcc
52:
	make all board=s3c2440a compiler=arm920t-gcc

# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc
92:
	make all board=posix compiler=posix-gcc debug=true
93:
	make all board=posix compiler=posix-gcc debug=false	
94:
	make dll board=posix compiler=posix-gcc debug=true	
	@mv -v $(exe-dir)/posix.dll $(exe-dir)/boot.dll

