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

obj-dir = $(CURDIR)/obj
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src
inc-dir = $(CURDIR)/include

prj-dir = $(subst release/aslua,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
LUA   = $(prj-dir)/com/as.tool/lua
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system
# --------------------------------- [ GENERAL SPECIFICATION ] ------------------------------------------- #		 
debug ?= TRUE
ifeq ($(host), Linux)
PY27 = python2
PY34 = python3
else
PY27 = C:Python27/python
PY34 = C:Python34/python
HOME = C:/Users/parai.wang
ifeq ($(HOME), $(wildcard $(HOME)))
else
$(error fix your home path)
endif
endif
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
DEPC  = $(PY27) $(prj-dir)/release/make/dep.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY27) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py

# --------------------------------- [ COMPILER SPECIFICATION ] ---------------------------------------- #		 
# on C disk do the following command:
## mklink /D IAR "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
include ../make/posix.mk
endif

# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #		 
ifeq ($(board),posix)
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
endif
ldflags-y += -lm -lpthread
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG -DUSE_SHELL

cflags-y +=-Wextra 
def-y    += -DLUA_COMPAT_5_2
dllflags-y += -Wl,--version-script=$(src-dir)/asvirtual.version
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
endif
ldflags-y += -lm -lpthread
cflags-y +=-Wextra 
def-y    += -D__AS_PY_CAN__
endif	# pyas end

inc-y += -I$(inc-dir)
def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board
def-y += -DUSE_DET

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

dep-posix:
	@(cd $(src-dir);$(LNFS) $(LUA)/src TRUE)
	@(cd $(src-dir);$(LNFS) $(LUA)/can TRUE)
	@(cd $(src-dir);$(LNFS) $(LUA)/main.c)
	@(cd $(src-dir);$(LNFS) $(LUA)/laslib.c)
	@(cd $(src-dir);$(LNFS) $(VIRTUAL)/source/asvirtual.version)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/shell TRUE)
ifeq ($(host),Linux)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/strtok_r.c)
	@(cd $(src-dir);cp C:/Users/parai.wang/Downloads/peak-can/PCAN-Basic\ API/Win32/VC_LIB/PCANBasic.lib PCANBasic.lib)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Det/asdebug.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/RPmsg FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/RPmsg_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/RPmsg_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Rproc_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Rproc_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/VirtQ_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/VirtQ_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Ipc.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Ipc.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Ipc_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Ipc_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/xlLoadlib.cpp xlLoadlib.c)
	@(echo "  >> prepare link for posix done")

$(exe-dir)/py27:
	@mkdir -p $@
	
$(exe-dir)/py34:
	@mkdir -p $@

# check http://pyqt.sourceforge.net/Docs/sip4/installation.html	
pysip:$(exe-dir)/py27 $(exe-dir)/py34
	@(cd $(exe-dir)/py27;sed -e "5c %Module as27" $(src-dir)/pyas.sip > pyas.sip)
	@(cd $(exe-dir)/py27;$(LNFS) $(src-dir)/pycan.h)
	@(cd $(exe-dir)/py27;$(LNFS) $(src-dir)/PCANBasic.lib libPCANBasic.a)
	@(cd $(exe-dir)/py27;$(PY27) $(src-dir)/configure.py pyas.sip $(exe-dir)/py27)
	@(cd $(exe-dir)/py34;sed -e "5c %Module as34" $(src-dir)/pyas.sip > pyas.sip)
	@(cd $(exe-dir)/py34;$(LNFS) $(src-dir)/pycan.h)
	@(cd $(exe-dir)/py34;$(LNFS) $(src-dir)/PCANBasic.lib libPCANBasic.a)
	@(cd $(exe-dir)/py34;$(PY34) $(src-dir)/configure.py pyas.sip $(exe-dir)/py34)	
	@echo "  >> pysip build done "

dep-pyas:
	@(cd $(src-dir);$(LNFS) $(LUA)/can TRUE)
	@(cd $(src-dir);$(LNFS) $(LUA)/pyas TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Det/asdebug.c)
ifeq ($(host),Linux)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/strtok_r.c)
	@(cd $(src-dir);cp C:/Users/parai.wang/Downloads/peak-can/PCAN-Basic\ API/Win32/VC_LIB/PCANBasic.lib PCANBasic.lib)
endif
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/xlLoadlib.cpp xlLoadlib.c)	
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(make pysip)
	
	
dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board)

all:exe

depc:
	@($(DEPC) $(obj-dir) $(obj-dir)/as.dep)

clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*

clean-dist:clean
	@rm -fv $(src-dir)/*
	@rm -fv $(inc-dir)/*

00:clean-dist
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc
92:
	make all board=posix compiler=posix-gcc debug=true
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
	@cp $(exe-dir)/*.a $(exe-dir)/py27
	@cp $(exe-dir)/*.a $(exe-dir)/py34
	@(cd $(exe-dir)/py27;make all)
	@(cd $(exe-dir)/py34;make all)
	@(cp -v $(exe-dir)/py27/*.pyd $(prj-dir)/com/as.tool/lua/script)
	@(cp -v $(exe-dir)/py34/*.pyd $(prj-dir)/com/as.tool/lua/script)
	@(cd $(prj-dir)/com/as.tool/as.one.py; $(LNFS) $(prj-dir)/com/as.tool/lua/script pyas)
