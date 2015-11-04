target-y = $(board)

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src
inc-dir = ./include

prj-dir = $(subst release/asboot,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
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
XCC   = $(PY34) xcc.py
STUDIO = $(PY27) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py

#rtos?=freeosek
#rtos?=freertos
rtos?=toppers_osek
# --------------------------------- [ COMPILER SPECIFICATION ] ---------------------------------------- #		 
# on C disk do the following command:
## mklink /D IAR "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
include ../make/posix.mk
endif

ifeq ($(compiler),arm-none-eabi-gcc)
ifeq ($(host), Linux)
#COMPILER_PREFIX = arm-linux-gnueabi-
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
CMSIS = /media/parai/OS/gcc-arm-none-eabi-4_8-2014q1-20140314-win32/CMSIS
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32

#COMPILER_PREFIX = arm-linux-gnueabihf-
#COMPILER_DIR = C:/opt/Linaro/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04
CMSIS = $(COMPILER_DIR)/CMSIS
endif
inc-y += -I$(inc-dir)/CMSIS/Include
include ../make/cortexm3.gcc.mk
endif

ifeq ($(compiler),iccarm)
COMPILER_DIR = C:/IAR
include ../make/cortexm3.iar.mk
endif

# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #		 
ifeq ($(board),posix)
ifeq ($(host),Linux)
def-y += -D__LINUX__
# my computer is 64 bit 
def-y += -D__X86_64__
else
def-y += -D__WINDOWS__
endif	
# slect GUI openVG/GTK 
ifeq ($(host),Linux)
ldflags-y +=-lrt
else
ldflags-y +=-lwinmm 
endif
ldflags-y += -lstdc++ -lm	
endif	# posix end

ifeq ($(board),stm32f107vc)
ifeq ($(compiler),arm-none-eabi-gcc)
link-script = $(src-dir)/linker.lds
endif
ifeq ($(compiler),iccarm)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += 
endif

inc-y += -I$(inc-dir)
def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@
	
XCC:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(prj-dir)/release/asboot/src false)	
	

dep-posix: 
	@(echo "  >> prepare link for posix done")

dep-stm32f107vc: 
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/boot TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/script/linker.lds linker.lds)
	@(echo "  >> prepare link for STM32F107VC done")		

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) XCC

all:exe

clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-dist:clean
	@rm -fv $(src-dir)/*
	@rm -fv $(inc-dir)/*
	
00:clean-dist
# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=arm-none-eabi-gcc
12:
	make all board=stm32f107vc compiler=arm-none-eabi-gcc debug=true
13:
	make all board=stm32f107vc compiler=arm-none-eabi-gcc debug=false	
	
15:
	make dep board=stm32f107vc compiler=iccarm
16:
	make all board=stm32f107vc compiler=iccarm debug=true
17:
	make all board=stm32f107vc compiler=iccarm debug=false
	
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc
92:
	make all board=posix compiler=posix-gcc debug=true
93:
	make all board=posix compiler=posix-gcc debug=false	
94:
	make dll board=posix compiler=posix-gcc debug=true	
	
	


