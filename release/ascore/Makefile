target-y = $(board)

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src

prj-dir = $(subst release/ascore,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system
# --------------------------------- [ GENERAL SPECIFICATION ] ------------------------------------------- #		 
DEBUG ?= TRUE
ifeq ($(host), Linux)
PY27 = python2
PY34 = python3
else
PY27 = C:Python27/python
PY34 = C:Python34/python
endif
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY27) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py
# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #		 
ifeq ($(host),Linux)
def-y += -D__LINUX__
else
def-y += -D__WINDOWS__
endif
# on C disk do the following command:
## mklink /D IAR "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
ifeq ($(compiler),posix-gcc)
#cflags-y += -Werror
COMPILER_DIR = 
include ../make/posix.mk
endif

ifeq ($(compiler),arm-none-eabi-gcc)
ifeq ($(host), Linux)
COMPILER_DIR = /usr
CMSIS = /media/parai/OS/gcc-arm-none-eabi-4_8-2014q1-20140314-win32/CMSIS
else
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
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
# slect GUI openVG/GTK 
ifeq ($(host),Linux)
else
ldflags-y +=-lwinmm 
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
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR 
# inter processor communication		 
def-y += -DUSE_IPC -DUSE_VIRTQ -DUSE_RPMSG

rtos?=freeosek
#rtos?=freertos
#rtos?=toppers_osek

ifeq ($(gui),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 
def-y += -DGUI_USE_OPENVG	
def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY
inc-y += -I./include/EGL
inc-y += -I./include/VG	
endif
endif	# posix end

ifeq ($(board),at91sam3s)
ifeq ($(DEBUG),TRUE)
link-script = $(COMPILER_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-sram.icf
else
link-script = $(COMPILER_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-flash.icf
endif
def-y += -DCHIP_AT91SAM3S -Dsam3s4 -DUSBD_LEDUSB=LED_RED
inc-y += -I./include/libraries/libchip_sam3s
inc-y += -I./include/libraries/libchip_sam3s/include
inc-y += -I./include/libraries/libboard_sam3s-ek
inc-y += -I./include/libraries/libboard_sam3s-ek/include
inc-y += -I./include/libraries
inc-y += -I./include/libraries/usb/include
inc-y += -I./include/libraries/usb/common/core
inc-y += -I./include/libraries/usb/common/cdc
inc-y += -I./include/libraries/usb/device/core
inc-y += -I./include/libraries/usb/device/cdc-serial
endif	# AT91SAM3S end

ifeq ($(board),stm32f107vc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM
endif	# stm32f107vc end

inc-y += -I./include
inc-dir = ./include

def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board

$(inc-dir)/utility:
	@mkdir -p $@
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

$(ASCORE)/SgDesign/SgRes/SgRes.h:
	@(cd SgDesign;$(SG) Sg.xml)	

SG:
	@(cd SgDesign;$(SG) Sg.xml)
	

XCC:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(prj-dir)/release/ascore/src false)	
# as the bug of FreeOSEK generator
	@(mv -v srcme/parai/workspace/as/release/ascore/src/* $(src-dir))
	@(rm -fvr srcme/parai/workspace/as/release/ascore/src)	

BSW:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) $(prj-dir)/release/ascore/src true)
	
studio:
	@(cd $(src-dir)/config.infrastructure.system;$(STUDIO) $(prj-dir)/release/ascore/src)	
	

dep-as-virtual:
	@($(LNFS) $(VIRTUAL) virtual)
	
dep-posix: $(ASCORE)/SgDesign/SgRes/SgRes.h dep-as-virtual
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app/config.autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common TRUE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)

	@(cd $(src-dir); rm infrastructure.xml)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/SgRes TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
#	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/vm TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)	
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/EGL)
endif
	@(cd $(src-dir);rm -f Dem.c J1939Tp* Lin*)
	@(make BSW)
	@(echo "  >> prepare link for posix done")

dep-stm32f107vc:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app/config.autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common TRUE)
	@(cd $(src-dir);sed -e "5c <OsRef name='toppers_osek'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);rm -f EcuM_Flexible.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m TRUE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cd $(src-dir);rm -f Dem* J1939Tp* Lin* Can* Com.* Com_* ComM* PduR* Nm* RPmsg* Ipc* VirtQ* Rproc* diag_app.c)
	@(echo "  >> prepare link for STM32F107VC done")
		
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
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

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) XCC
	
all: exe

$(board)-dll: clean dll

$(board)-run: 
	@($(PY27) Simulator.py)
clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-dist:clean
	@rm -fv $(src-dir)/*
	
00:clean-dist
# ---------------------------------- [ AT91SAM3S   ] ---------------------------------- #
01:
	make dep board=at91sam3s compiler=iccarm
02:
	make all board=at91sam3s compiler=iccarm DEBUG=TRUE
03:
	make all board=at91sam3s compiler=iccarm DEBUG=FALSE	
# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=arm-none-eabi-gcc
12:
	make all board=stm32f107vc compiler=arm-none-eabi-gcc DEBUG=TRUE
13:
	make all board=stm32f107vc compiler=arm-none-eabi-gcc DEBUG=FALSE	
# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc gui=GTK
92:
	make all board=posix compiler=posix-gcc gui=GTK DEBUG=TRUE
93:
	make all board=posix compiler=posix-gcc gui=GTK DEBUG=FALSE	
94:
	make dll board=posix compiler=posix-gcc gui=GTK DEBUG=TRUE	


