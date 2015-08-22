target-y = $(board)

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src

prj-dir = $(subst release/ascore,,$(CURDIR))

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system

PY27 = C:/Python27/python.exe
ifeq ($(PY27), $(wildcard $(PY27)))
else
$(error fix your python27 path)
endif

PY34 = C:/Python34/python.exe
ifeq ($(PY34), $(wildcard $(PY34)))
else
$(error fix your python34 path)
endif

LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
XCC   = $(PY34) xcc.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py
# on C disk do the following command:
## mklink /D IAR "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
IAR_DIR = C:/IAR
inc-y += -I./include
inc-dir = ./include
		 
ifeq ($(board),mingw)	
# slect GUI openVG/GTK 
ldflags-y += -lm -lwinmm 
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0` \
			 -lpthread
ldflags-y += -lstdc++	

def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM -DUSE_DET -DUSE_GUI	\
		 -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_CANTP

ifeq ($(gui),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 
def-y += -DGUI_USE_OPENVG	
def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY
inc-y += -I./include/EGL
inc-y += -I./include/VG	
endif
endif	# MINGW end

ifeq ($(board),at91sam3s)
ifeq ($(DEBUG),TRUE)
link-script = $(IAR_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-sram.icf
else
link-script = $(IAR_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-flash.icf
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
obj-y += $(obj-dir)/startup.o $(obj-dir)/portableS.o 
endif	# AT91SAM3S end

ifeq ($(board),stm32f107vc)
link-script = $(IAR_DIR)/arm/config/linker/ST/stm32f107xC.icf
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM
obj-y += $(obj-dir)/startup.o $(obj-dir)/portableS.o 
endif	# stm32f107vc end

def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board


ifeq ($(board),mingw)
#cflags-y += -Werror
include ../make/mingw.mk
endif

ifeq ($(board),at91sam3s)
include ../make/cortexm3.mk
endif

ifeq ($(board),stm32f107vc)
include ../make/cortexm3.mk
endif


$(inc-dir)/utility:
	@mkdir -p $@
	
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@

$(ASCORE)/SgDesign/SgRes/SgRes.h:
	@(cd SgDesign;$(SG) Sg.xml)	

XCC:
	@(cd $(src-dir)/config.infrastructure.system;$(XCC) ..)	
	
dep-mingw: $(ASCORE)/SgDesign/SgRes/SgRes.h
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app/config.autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common TRUE)
	@(cd $(src-dir);sed -e "5c <OsRef name='freertos'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(ASCORE)/SgDesign/SgRes TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/ecum TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/vm TRUE)
	@(cd $(src-dir);rm -f EcuM_Flexible.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/schm TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MemMang FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MSVC-MingW FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c)
ifeq ($(gui),openVG)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/EGL)
endif
	@(echo "  >> prepare link for MINGW done")

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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/ecum TRUE)
	@(cd $(src-dir);rm -f EcuM_Flexible.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/schm TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m TRUE)
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
	@(echo "  >> prepare link for AT91SAM3S done")

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) XCC
	
all: exe

$(board)-dll: dll

$(board)-run: 
	@($(PY27) Simulator.py)
clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-dist:clean
	@rm -fv $(src-dir)/*
	
	
 

