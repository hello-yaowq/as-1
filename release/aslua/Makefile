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

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src
inc-dir = ./include

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
endif	
ifeq ($(host),Linux)
ldflags-y +=-lrt
ldflags-y +=  -ldl -lreadline
else
ldflags-y +=-lwinmm 
endif
ldflags-y += -lstdc++ -lm -lpthread
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG -DUSE_SHELL

cflags-y +=-Wextra 
def-y    += -DLUA_COMPAT_5_2
dllflags-y += -Wl,--version-script=$(src-dir)/asvirtual.version

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=
endif	# posix end

inc-y += -I$(inc-dir)
def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board

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
	