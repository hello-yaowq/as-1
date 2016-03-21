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

obj-dir = $(CURDIR)/obj/$(board)-$(rtos)-$(compiler)
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src/$(board)-$(rtos)-$(compiler)
inc-dir = $(CURDIR)/src/$(board)-$(rtos)-$(compiler)/include

prj-dir = $(subst /release/ascore,,$(CURDIR))

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
HOME = /home/parai
else
PY27 = C:Python27/python
PY34 = C:Python34/python
HOME = C:/Users/parai.wang
endif
ifeq ($(HOME), $(wildcard $(HOME)))
else
$(error fix your home path)
endif
LUA   = $(prj-dir)/com/as.tool/lua
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
DEPC  = $(PY34) $(prj-dir)/release/make/dep.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY34) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py
S19   = $(PY34) $(prj-dir)/com/as.tool/lua/script/s19.py
#sgapp ?= watch
sgapp ?= virtual_cluster
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

ifeq ($(compiler),arm920t-gcc)
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
include ../make/arm920t.gcc.mk
endif

# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #		 
ifeq ($(board),posix)
#rtos?=freeosek
#rtos?=freertos
#rtos?=toppers_osek
rtos?=smallos
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
ldflags-y +=-ldl -lreadline
else
ldflags-y +=-lwinmm -lwsock32
ldflags-y += $(src-dir)/PCANBasic.lib
endif
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0` \
			 -lpthread
ldflags-y += -lstdc++ -lm	

# basic
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM -DUSE_DET
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM -DUSE_OSEKNM
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG
# memory service
def-y += -DUSE_FLS -DUSE_EEP -DUSE_EA -DUSE_FEE -DUSE_NVM -DUSE_MEMIF
dllflags-y += -Wl,--version-script=$(src-dir)/asvirtual.version
def-y += -DUSE_GUI -DUSE_STMO
def-y += -D__AS_CAN_BUS__
ifeq ($(gui),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 # openVG
def-y += -DGUI_USE_OPENVG	
def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY
inc-y += -I$(inc-dir)/EGL
inc-y += -I$(inc-dir)/VG	
endif
endif	# posix end

ifeq ($(board),at91sam3s)
#rtos?=freeosek
#rtos?=freertos
rtos?=toppers_osek
#rtos?=smallos
ifeq ($(DEBUG),TRUE)
link-script = $(COMPILER_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-sram.icf
else
link-script = $(COMPILER_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-flash.icf
endif
def-y += -DCHIP_AT91SAM3S -Dsam3s4 -DUSBD_LEDUSB=LED_RED
inc-y += -I$(inc-dir)/libraries/libchip_sam3s
inc-y += -I$(inc-dir)/libraries/libchip_sam3s/include
inc-y += -I$(inc-dir)/libraries/libboard_sam3s-ek
inc-y += -I$(inc-dir)/libraries/libboard_sam3s-ek/include
inc-y += -I$(inc-dir)/libraries
inc-y += -I$(inc-dir)/libraries/usb/include
inc-y += -I$(inc-dir)/libraries/usb/common/core
inc-y += -I$(inc-dir)/libraries/usb/common/cdc
inc-y += -I$(inc-dir)/libraries/usb/device/core
inc-y += -I$(inc-dir)/libraries/usb/device/cdc-serial
endif	# AT91SAM3S end

ifeq ($(board),stm32f107vc)
#rtos?=freeosek
#rtos?=freertos
rtos?=toppers_osek
#rtos?=smallos
use-boot?=yes
ifeq ($(compiler),cortexm3-gcc)
ifeq ($(use-boot),yes)
link-script = $(src-dir)/linker-app.lds
else
link-script = $(src-dir)/linker-common.lds
endif
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM
#def-y += -DUSE_DET
def-y += -DUSE_SIMUL_CAN
endif	# stm32f107vc end

ifeq ($(board),lm3s6965evb)
#rtos?=freeosek
#rtos?=freertos
rtos?=toppers_osek
#rtos?=smallos
ifeq ($(compiler),cortexm3-gcc)
link-script = $(src-dir)/linker.lds
else
$(error compiler arm gcc not supported for board lm3s6965evb)
endif
def-y += -DCHIP_LM3S6965
def-y += -DUSE_KERNEL -DUSE_ECUM -DUSE_SCHM -DUSE_MCU 
def-y += -DUSE_DET
inc-y += -I$(src-dir)/library -I$(src-dir)/library/src
ldflags-y += -L$(src-dir)/library -llm3sdriver
endif	# lm3s6965evb end

ifeq ($(board),s3c2440a)
#rtos?=freeosek
#rtos?=freertos
#rtos?=toppers_osek
#rtos?=smallos
rtos?=atk2-sc4
ifeq ($(compiler),arm920t-gcc)
link-script = $(src-dir)/linker-common.lds
else
$(error compiler arm gcc not supported for board lm3s6965evb)
endif
def-y += -DCHIP_S3C2440A
def-y += -DUSE_KERNEL -DUSE_ECUM -DUSE_SCHM -DUSE_MCU
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM
def-y += -DUSE_DET
def-y += -DTEXT_BASE=0x30000000
endif	# lm3s6965evb end

def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board
inc-y += -I$(inc-dir)

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

$(ASCORE)/SgDesign/SgRes/SgRes.h:
	@(cd SgDesign;$(SG) $(sgapp)/Sg.xml)	

SG:
	@(cd SgDesign;$(SG) $(sgapp)/Sg.xml)
	

OS:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) false)	
ifeq ($(rtos),freeosek)
# as the bug of FreeOSEK generator
	@(mv -v srcme/parai/workspace/as/release/ascore/src/* $(src-dir))
	@(rm -fvr srcme/parai/workspace/as/release/ascore/src)	
endif

BSW:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(src-dir) true)
	
studio:
	@(cd $(src-dir)/config.infrastructure.system;$(STUDIO) $(src-dir))	
	

dep-as-virtual:
	@($(LNFS) $(VIRTUAL) virtual)
	
dep-posix: SG dep-as-virtual
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(VIRTUAL)/source/asvirtual.version)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/SgRes TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/$(sgapp)/src TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/Crc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/memory TRUE)
#	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/vm TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/cirq_buffer.c)
ifeq ($(rtos),smallos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/posix FALSE)
endif
ifeq ($(rtos),atk2-sc4)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/posix FALSE)
endif
ifeq ($(rtos),freeosek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/inc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/OpenGEN)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src/posix TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc/posix FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src/posix FALSE)
else	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/src/win TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/inc/win FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freeosek/gen/src/win FALSE)
endif	
endif 	
ifeq ($(rtos),freertos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MemMang FALSE)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/GCC/Posix FALSE)
else	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MSVC-MingW FALSE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c)
endif
ifeq ($(rtos),toppers_osek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
ifeq ($(host),Linux)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/posix TRUE)
else	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/mingw TRUE)
endif	
endif 
	
ifeq ($(gui),openVG)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/EGL)
endif
	@(cd $(src-dir);$(LNFS) $(LUA)/can TRUE)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/as.tool/cancasexl.access/xlLoadlib.cpp xlLoadlib.c)
ifeq ($(host),Linux)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/strtok_r.c)
	@(cd $(src-dir);cp C:/Users/parai.wang/Downloads/peak-can/PCAN-Basic\ API/Win32/VC_LIB/PCANBasic.lib PCANBasic.lib)
endif
	@(cd $(src-dir);rm -f Dem.c J1939Tp* Lin*)
	@(make BSW)
	@(echo "  >> prepare link for posix done")

dep-stm32f107vc:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/Crc TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
ifeq ($(rtos),smallos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m/gcc FALSE)	
else
	$(error for small os, non-gcc compiler is not supported)
endif
endif
ifeq ($(rtos),freertos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MemMang FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/GCC/ARM_CM3 FALSE)
endif
ifeq ($(compiler),cortexm3-icc)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/IAR/ARM_CM3 FALSE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_3.c heap_5.c)
endif
ifeq ($(rtos),toppers_osek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/gcc FALSE)
endif
ifeq ($(compiler),cortexm3-icc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/icc FALSE)
endif
endif
ifeq ($(rtos),atk2-sc4)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/armv7_m FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/armv7_m/gcc TRUE)
else
	$(error for atk2-sc4 os, non-gcc compiler is not supported)
endif
endif
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
endif
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cp -fv $(src-dir)/rom.gdbinit $(HOME)/.gdbinit)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make BSW)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Lcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Cfg.h)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for STM32F107VC done")

dep-lm3s6965evb-library:	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/lm3s/DriverLib library)
	@(cd $(src-dir)/library; make all)
	
dep-lm3s6965evb:dep-lm3s6965evb-library
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common TRUE)
	@(cd $(src-dir);sed -e "5c <OsRef name='toppers_osek'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic/Det TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);rm -f EcuM_Flexible.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/lm3s/mcal TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/gcc FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
endif
ifeq ($(compiler),cortexm3-icc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/icc FALSE)
endif
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cd $(src-dir);rm -f Dem* J1939Tp* Lin* Can* Com.* Com_* ComM* PduR* Nm* RPmsg* Ipc* VirtQ* Rproc* diag_app.c)
	@(echo "  >> prepare link for lm3s6965evb done")
		
dep-at91sam3s:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common TRUE)
	@(cd $(src-dir);sed -e "5c <OsRef name='toppers_osek'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)		
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libboard_sam3s-ek TRUE)
	@(cd $(src-dir);rm -f supc.c syscalls.c board_cstartup_iar.c hsmci_pdc.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/common/core TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/common/cdc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/device/core TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/device/cdc-serial TRUE)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s/include/trace.h trace.h)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libboard_sam3s-ek/include/led.h led.h)
	@(cd $(inc-dir)/utility;$(LNFS) $(IAR_DIR)/arm/inc/c/assert.h assert.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m TRUE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(echo "  >> prepare link for AT91SAM3S done")

dep-s3c2440a:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/s3c2440a/mcal TRUE)
ifeq ($(compiler),arm920t-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
endif
ifeq ($(rtos),atk2-sc4)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/arm920t FALSE)
ifeq ($(compiler),arm920t-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/arm920t/gcc TRUE)
else
	$(error for atk2-sc4 os, non-gcc compiler is not supported)
endif
endif
	@(make BSW)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for S3C2440A done")
	
dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) OS

all:exe

depc:
	@($(DEPC) $(obj-dir) $(obj-dir)/as.dep)

clean: 
	@rm -frv ./obj/*
	@rm -fv $(ASCORE)/SgDesign/SgRes/*

# ---------------------------------- [ AT91SAM3S   ] ---------------------------------- #
01:
	make dep board=at91sam3s compiler=cortexm3-icc
02:
	make all board=at91sam3s compiler=cortexm3-icc debug=true
03:
	make all board=at91sam3s compiler=cortexm3-icc debug=false	
# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=cortexm3-gcc rtos=atk2-sc4
12:
	make all board=stm32f107vc compiler=cortexm3-gcc rtos=atk2-sc4 debug=true
ifeq ($(use-boot),yes)
	@($(S19) -m ../asboot/out/stm32f107vc.s19 out/stm32f107vc.s19 -o out/boot_app.s19)
endif
13:
	make all board=stm32f107vc compiler=cortexm3-gcc rtos=atk2-sc4 debug=false	

15:
	make dep board=stm32f107vc compiler=cortexm3-icc
16:
	make all board=stm32f107vc compiler=cortexm3-icc debug=true
17:
	make all board=stm32f107vc compiler=cortexm3-icc debug=false
	
# ---------------------------------- [ lm3s6965evb  ] ---------------------------------- #
21:
	make dep board=lm3s6965evb compiler=cortexm3-gcc
22:
	make all board=lm3s6965evb compiler=cortexm3-gcc debug=true
23:
	make all board=lm3s6965evb compiler=cortexm3-gcc debug=false
		
		
29:
	@echo "target remote:2345" > ~/.gdbinit
	@echo "file out/lm3s6965evb.exe" >> ~/.gdbinit
	@echo "b main" >> ~/.gdbinit
	@echo "b usage_fault_handler" >> ~/.gdbinit
	
	@echo "#! /bin/sh" > gdbg.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> gdbg.sh
	@echo "qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial stdio -semihosting -machine lm3s6965evb -kernel out/lm3s6965evb.exe -gdb tcp::2345 -S &" >> gdbg.sh
	@echo "echo target remote:2345 b main c s n" >> gdbg.sh
	@echo "arm-none-eabi-gdb" >> gdbg.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> gdbg.sh
	@echo "echo end of debug" >> gdbg.sh
	@chmod +x gdbg.sh
	
	@echo "#! /bin/sh" > run.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> run.sh
	@echo "qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial stdio -semihosting -machine lm3s6965evb -kernel out/lm3s6965evb.exe" >> run.sh
	@echo "echo end of running" >> run.sh
	@chmod +x run.sh

# ---------------------------------- [ s3c2440a    ] ---------------------------------- #
31:
	make dep board=s3c2440a compiler=arm920t-gcc rtos=atk2-sc4
32:
	make all board=s3c2440a compiler=arm920t-gcc rtos=atk2-sc4 debug=true
33:
	make all board=s3c2440a compiler=arm920t-gcc rtos=atk2-sc4 debug=false	
	 
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc gui=GTK rtos=smallos
92:
	make all board=posix compiler=posix-gcc gui=GTK rtos=smallos debug=true
93:
	make all board=posix compiler=posix-gcc gui=GTK rtos=smallos debug=false	
94:
	make dll board=posix compiler=posix-gcc gui=GTK rtos=smallos debug=true	
	@cp -v ./out/posix.dll ../asboot/out/core.dll


