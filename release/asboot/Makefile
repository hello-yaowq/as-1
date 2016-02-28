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
STUDIO = $(PY34) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py

use_nvm?=no
led_app?=yes
# --------------------------------- [ COMPILER SPECIFICATION ] ---------------------------------------- #		 
# on C disk do the following command:
## mklink /D IAR "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
include ../make/posix.mk
endif

ifeq ($(compiler),cortexm3-gcc)
CMSIS = $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS
ifeq ($(host), Linux)
#COMPILER_PREFIX = arm-linux-gnueabi-
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32

#COMPILER_PREFIX = arm-linux-gnueabihf-
#COMPILER_DIR = C:/opt/Linaro/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04
endif
inc-y += -I$(inc-dir)/CMSIS/Include
include ../make/cortexm3.gcc.mk
endif

ifeq ($(compiler),cortexm3-icc)
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
def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=
endif	
# slect GUI openVG/GTK 
ifeq ($(host),Linux)
ldflags-y +=-lrt
else
ldflags-y +=-lwinmm -lwsock32
ldflags-y += $(src-dir)/PCANBasic.lib
endif
ldflags-y += -lstdc++ -lm -lpthread
def-y += -DUSE_DET -DUSE_MCU -DUSE_SCHM -DUSE_ECUM -DUSE_KERNEL
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PDUR -DUSE_CANTP -DUSE_DCM	
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG
# memory service
ifeq ($(use_nvm),yes)
def-y += -DUSE_FLS -DUSE_EEP -DUSE_EA -DUSE_FEE -DUSE_NVM -DUSE_MEMIF
endif
def-y += -D__AS_CAN_BUS__
dllflags-y += -Wl,--version-script=$(src-dir)/asvirtual.version
endif	# posix end

ifeq ($(board),stm32f107vc)
ifeq ($(compiler),cortexm3-gcc)
link-script = $(src-dir)/linker-boot.lds
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_MCU -DUSE_SCHM -DUSE_ECUM -DUSE_KERNEL
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PDUR -DUSE_CANTP -DUSE_DCM
ifeq ($(led_app),yes)
def-y += -DUSE_DIO -DUSE_PORT
endif
#def-y += -DUSE_DET
def-y += -DUSE_SIMUL_CAN
endif

ifeq ($(board),stm32f107vc-flsdrv)
ifeq ($(compiler),cortexm3-gcc)
link-script = $(src-dir)/linker-flsdrv.lds
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DFLASH_DRIVER_DYNAMIC_DOWNLOAD
endif

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

dep-as-virtual:
	@($(LNFS) $(VIRTUAL) virtual)
	
dep-posix: dep-as-virtual
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(VIRTUAL)/source/asvirtual.version)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/posix FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
ifeq ($(use_nvm),yes)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/Crc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/memory TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/MemIf_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Crc_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Fls.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Eep.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/test TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Fee_Memory_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/cirq_buffer.c)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/CanIf TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/CanTp TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/PduR TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/RPmsg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Det TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Dcm TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/EcuM_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/EcuM_PBcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/SchM_cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Det_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/RPmsg_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/RPmsg_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Rproc_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Rproc_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/VirtQ_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/VirtQ_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/CanIf_SpecialPdus.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/boot/common TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Flash.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Can.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Mcu.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_ConfigTypes.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Ipc.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal/Ipc.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Ipc_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Ipc_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/simulator TRUE)
	@(cd $(src-dir);$(LNFS) $(LUA)/can TRUE)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/xlLoadlib.cpp xlLoadlib.c)
ifeq ($(host),Linux)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/strtok_r.c)
	@(cd $(src-dir);cp C:/Users/parai.wang/Downloads/peak-can/PCAN-Basic\ API/Win32/VC_LIB/PCANBasic.lib PCANBasic.lib)
endif	
	@(make BSW)
	@(echo "  >> prepare link for posix done")

dep-stm32f107vc: 
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m/gcc FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/CanIf TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/CanTp TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication/PduR TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Det TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Dcm TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/EcuM_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/EcuM_PBcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/SchM_cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/Det_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config/CanIf_SpecialPdus.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/boot/common TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Flash.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Can.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Mcu.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_ConfigTypes.h)
ifeq ($(led_app),yes)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/leds TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Dio.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Port.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Dio_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Dio_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_ConfigTypes.h)
endif
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/script TRUE)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make BSW)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Lcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Cfg.h)
	@(echo "  >> prepare link for STM32F107VC done")		

dep-stm32f107vc-flsdrv: 
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Flash.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.stm32f107vc/script TRUE)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	
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

