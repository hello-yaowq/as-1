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

export forceclib?=no

obj-dir = $(CURDIR)/obj/$(board)
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src/$(board)
inc-dir = $(CURDIR)/src/$(board)/include
download = $(CURDIR)/../download

#export rootfs ?= ~/workspace/qemu-vexpress-ca9/out/rootfs
export rootfs ?= $(shell readlink -f $(CURDIR)/../aslinux/build/out/rootfs)

prj-dir ?= $(subst release/aslua,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
export LUA = $(prj-dir)/com/as.tool/lua
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system
# --------------------------------- [ GENERAL SPECIFICATION ] ------------------------------------------- #		 
debug ?= TRUE
ifeq ($(host), Linux)
PY ?= python3
SCONS ?= scons
else
PY ?= python
SCONS ?= scons.bat
ifeq ($(shell which gcc),/mingw64/bin/gcc)
export ISMINGW64=YES
endif
endif
LNFS  = $(PY) $(prj-dir)/release/make/lnfs.py
DEPC  = $(PY) $(prj-dir)/release/make/dep.py
XCC   = $(PY) xcc.py
STUDIO = $(PY) studio.py
SG    = $(PY) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py
# --------------------------------- [ OBJECT SPECIFICATION ] ---------------------------------------- #
ifeq ($(board),posix)
dir-y += $(LUA)
dir-y += $(LUA)/src
dir-y += $(LUA)/can
dir-y += $(LUA)/can/rs232
dir-y += $(LUA)/device
dir-y += $(INFRASTRUCTURE)/system/shell
dir-y += $(INFRASTRUCTURE)/libraries/ringbuffer
dir-y += $(INFRASTRUCTURE)/communication/RPmsg

inc-y += -I$(INFRASTRUCTURE)/include
inc-y += -I$(INFRASTRUCTURE)/include/sys
inc-y += -I$(APPLICATION)/common/config
inc-y += -I$(APPLICATION)/board.$(board)/common
inc-y += -I$(prj-dir)/com/as.tool/cancasexl.access
inc-y += -I$(INFRASTRUCTURE)/arch/posix/mcal

VPATH += $(APPLICATION)/common/config \
	$(APPLICATION)/board.$(board)/common \
	$(INFRASTRUCTURE)/arch/posix/mcal \
	$(INFRASTRUCTURE)/diagnostic/Det

src-y += $(APPLICATION)/common/config/Rproc_Cfg.c
src-y += $(APPLICATION)/common/config/VirtQ_Cfg.c
src-y += $(APPLICATION)/common/config/RPmsg_Cfg.c
src-y += $(INFRASTRUCTURE)/arch/posix/mcal/Ipc.c
src-y += $(APPLICATION)/board.$(board)/common/Ipc_Cfg.c
src-y += $(INFRASTRUCTURE)/diagnostic/Det/asdebug.c
ifeq ($(host),Linux)
else
VPATH += $(INFRASTRUCTURE)/clib
src-y += $(INFRASTRUCTURE)/clib/strtok_r.c
endif
endif
ifeq ($(board),pyas)
dir-y += $(LUA)/can
dir-y += $(LUA)/can/rs232
dir-y += $(LUA)/device
dir-y += $(LUA)/pyas
inc-y += -I$(INFRASTRUCTURE)/include
inc-y += -I$(INFRASTRUCTURE)/include/sys

VPATH += $(INFRASTRUCTURE)/diagnostic/Det
src-y += $(INFRASTRUCTURE)/diagnostic/Det/asdebug.c
ifeq ($(host),Linux)
else
VPATH += $(INFRASTRUCTURE)/clib
src-y += $(INFRASTRUCTURE)/clib/strtok_r.c
endif
endif
# --------------------------------- [ COMPILER SPECIFICATION ] ---------------------------------------- #
ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
ifeq ($(ISMINGW64),YES)
export AS  = gcc
export CC  = gcc
export AR  = ar
endif
include ../make/posix.mk
endif
# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #
def-y    += -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1
ifeq ($(board),posix)
ifeq ($(host),Linux)
def-y += -DLUA_USE_LINUX
def-y += -D__LINUX__
# my computer is 64 bit 
def-y += -D__X86_64__
else
def-y += -D__WINDOWS__
def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=
ifeq ($(ISMINGW64),YES)
def-y += -D__X86_64__
endif
def-y += -DFORCEINLINE=static\ inline
endif	
ifeq ($(host),Linux)
ldflags-y +=-lrt
ldflags-y +=  -ldl -lreadline
else
ldflags-y +=-lwinmm
ldflags-y += $(src-dir)/PCANBasic.lib
ldflags-y += $(src-dir)/ControlCAN.lib
endif
ldflags-y += -lm -lstdc++
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG -DUSE_SHELL

cflags-y +=-Wextra 
dllflags-y += -Wl,--version-script=$(VIRTUAL)/source/asvirtual.version
endif	# posix end

ifeq ($(board),pyas)
ifeq ($(host),Linux)
def-y += -DLUA_USE_LINUX
def-y += -D__LINUX__
# my computer is 64 bit 
def-y += -D__X86_64__
else
def-y += -D__WINDOWS__
def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=
endif
ifeq ($(host),Linux)
ldflags-y +=-lrt
ldflags-y +=  -ldl -lreadline
else
ldflags-y +=-lwinmm
ldflags-y += $(src-dir)/PCANBasic.lib
ldflags-y += $(src-dir)/ControlCAN.lib
endif
ldflags-y += -lm
cflags-y +=-Wextra 
ifeq ($(forceclib),yes)
def-y += -D__AS_CAN_BUS__
else
def-y    += -D__AS_PY_CAN__
endif
def-y    += -D__AS_PY_DEV__
endif	# pyas end

termux?=no
ifeq ($(termux),yes)
def-y += -D__TERMUX__
inc-y += -I$(download)
endif

inc-y += -I$(inc-dir)
def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board
def-y += -DUSE_DET

export USE_AWS?=YES

ifeq ($(USE_AWS),YES)
def-y += -DUSE_AWS
inc-y += -I$(LUA)/device/websock
ldflags-y += -L$(LUA)/device/websock/out -laws
endif

ldflags-y += -lpthread

ZLGCAN?=$(wildcard $(download)/CAN_lib/CAN*)

ifeq ($(host), Linux)
else
ldflags-y += -lwsock32
inc-y += -I$(download)/pcan-basic/"PCAN-Basic API"/Include

ifeq ($(ISMINGW64),YES)
inc-y += -I$(ZLGCAN)/ControlCAN/64
else
inc-y += -I$(ZLGCAN)/ControlCAN/32
endif
endif
# --------------------------------- [ BUILD ACTIONs ] ------------------------------------------- #
$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

cleanaws:
	@(cd $(LUA)/device/websock; make clean)

libaws:
ifeq ($(USE_AWS),YES)
	@(cd $(LUA)/device/websock; make all CC=$(COMPILER_PREFIX)gcc AR=$(COMPILER_PREFIX)ar)
endif

$(LUA)/device/websock/out/libaws.a:libaws

aslibaws:$(LUA)/device/websock/out/libaws.a

dep-canlib:
	@(cd $(prj-dir)/release/ascore; export BOARD=posix; ${SCONS} --canlib)
ifeq ($(host), Linux)
else
ifeq ($(ISMINGW64),YES)
	@(cp -fv $(ZLGCAN)/ControlCAN/64/ControlCAN.dll $(prj-dir)/com/as.tool/as.one.py)
	@(cp -fv $(download)/pcan-basic/"PCAN-Basic API"/x64/PCANBasic.dll $(prj-dir)/com/as.tool/as.one.py)
else
	@(cp -fv $(ZLGCAN)/ControlCAN/32/ControlCAN.dll $(prj-dir)/com/as.tool/as.one.py)
	@(cp -fv $(download)/pcan-basic/"PCAN-Basic API"/Win32/PCANBasic.dll $(prj-dir)/com/as.tool/as.one.py)
endif
	@(cp -fv $(ZLGCAN)/ControlCAN/64/ControlCAN.lib /mingw64/lib)
endif

dep-posix: aslibaws
ifeq ($(host),Linux)
else
ifeq ($(ISMINGW64),YES)
	@(cd $(src-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/x64/VC_LIB/PCANBasic.lib .)
	@(cd $(src-dir);cp -f $(ZLGCAN)/ControlCAN/64/ControlCAN.lib .)
else
	@(cd $(src-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/Win32/VC_LIB/PCANBasic.lib .)
	@(cd $(src-dir);cp -f $(ZLGCAN)/ControlCAN/32/ControlCAN.lib .)
endif
endif
	@(echo "  >> prepare link for posix done")

# check http://pyqt.sourceforge.net/Docs/sip4/installation.html	
pysip:$(exe-dir)
ifeq ($(host),Linux)
else
ifeq ($(ISMINGW64),YES)
	@(cd $(exe-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/x64/VC_LIB/PCANBasic.lib libPCANBasic.a)
	@(cd $(exe-dir);cp -f $(ZLGCAN)/ControlCAN/64/ControlCAN.lib libZLGCAN.a)
else
	@(cd $(exe-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/Win32/VC_LIB/PCANBasic.lib libPCANBasic.a)
	@(cd $(exe-dir);cp -f $(ZLGCAN)/ControlCAN/32/ControlCAN.lib libZLGCAN.a)
endif
endif
ifeq ($(USE_AWS),YES)
	@(cd $(exe-dir);cp -f $(LUA)/device/websock/out/libaws.a .)
endif
	@echo $(PY) $(LUA)/pyas/configure.py $(LUA)/pyas/pyas.sip $(exe-dir)
	@(cd $(exe-dir);$(PY) $(LUA)/pyas/configure.py $(LUA)/pyas/pyas.sip $(exe-dir))
	@echo "  >> pysip build done "

dep-pyas: aslibaws
ifeq ($(host),Linux)
else
ifeq ($(ISMINGW64),YES)
	@(cd $(src-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/x64/VC_LIB/PCANBasic.lib libPCANBasic.a)
	@(cd $(src-dir);cp -f $(ZLGCAN)/ControlCAN/64/ControlCAN.lib libZLGCAN.a)
else
	@(cd $(src-dir);cp -f $(download)/pcan-basic/"PCAN-Basic API"/Win32/VC_LIB/PCANBasic.lib libPCANBasic.a)
	@(cd $(src-dir);cp -f $(ZLGCAN)/ControlCAN/32/ControlCAN.lib libZLGCAN.a)
endif
endif
	@(make pysip)

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board)

can_socket:
ifeq ($(board),posix)
ifeq ($(host),Linux)
	@echo "  >> use linux vcan"
else
	@echo "  >> CC socketwin_can_driver.c"
	@gcc -D__SOCKET_WIN_CAN_DRIVER__ -D__WINDOWS__ -I$(INFRASTRUCTURE)/include $(LUA)/can/socketwin_can_driver.c $(ldflags-y) -o $(exe-dir)/socketwin_can_driver
	@cp -v $(exe-dir)/socketwin_can_driver.exe $(prj-dir)/com/as.tool/lua/script/
endif
endif

all:exe can_socket

depc:
	@($(DEPC) $(obj-dir) $(obj-dir)/as.dep)

clean: cleanaws
	@rm -frv $(obj-dir)/*
	@rm -frv $(exe-dir)/*

clean-dist:clean
	@rm -fv $(src-dir)/*
	@rm -fv $(inc-dir)/*

00:clean-dist
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc
92:
	make all board=posix compiler=posix-gcc debug=true
	@(cp -v $(exe-dir)/posix.exe $(prj-dir)/com/as.tool/lua/script/aslua.exe)
93:
	make all board=posix compiler=posix-gcc debug=false	
94:
	make dll board=posix compiler=posix-gcc debug=true
	@cp -v $(exe-dir)/posix.dll ../asboot/out/aslua.dll
	@(cp -v $(exe-dir)/posix.exe $(prj-dir)/com/as.tool/lua/script/aslua.exe)

# ---------------------------------- [ pyas        ] ---------------------------------- #
# lua is preferred by me for validation, as it is small and perfect
# but it is not able to make UI application
# so I start this for a solution based on the lua can.
81:
	make dep board=pyas compiler=posix-gcc
82:
	make lib board=pyas compiler=posix-gcc debug=true
ifeq ($(forceclib),yes)
else
ifeq ($(host), Linux)
	@(cd $(exe-dir);make all CXX="${CXX}")
else
	@(cd $(exe-dir);make all -B -f $(LUA)/pyas/Makefile)
endif
ifeq ($(host), Linux)	
	@(cp -v $(exe-dir)/*.so $(prj-dir)/com/as.tool/lua/script)
else
	@(cp -v $(exe-dir)/*.pyd $(prj-dir)/com/as.tool/lua/script)
endif
	@(cd $(prj-dir)/com/as.tool/as.one.py; rm pyas; $(LNFS) $(prj-dir)/com/as.tool/lua/script pyas)
	@(cp -v $(exe-dir)/libpyas.a $(VIRTUAL)/source/libpyas.so)
endif

$(download):
	mkdir -p $@

aslua: $(download) libaws dep-canlib 81 82 91 92

