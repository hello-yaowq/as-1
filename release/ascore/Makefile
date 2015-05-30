target-y = $(board)

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src

prj-dir = $(subst release/ascore,,$(CURDIR))

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure/

LNFS  = python $(prj-dir)/release/make/lnfs.py

dir-y += ./app
dir-y += ./app/config
inc-y += -I./include
inc-y += -I./app/config
inc-dir = ./include
		 
ifeq ($(board),mingw)	
# slect GUI openVG/GTK
gui-used = GTK	 
ldflags-y += -lm -lwinmm 
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0` \
			 -lpthread
ldflags-y += -lstdc++	

ifeq ($(gui-used),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 
def-y += -DGUI_USE_OPENVG	
def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY
endif
inc-y += -I./include/EGL
inc-y += -I./include/VG
dir-y += ./SgDesign/SgRes		
endif	# MINGW end

ifeq ($(board),at91sam3s)
ifeq ($(DEBUG),TRUE)
link-script = $(IAR_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-sram.icf
else
link-script = $(IAR_DIR)/arm/config/linker/Atmel/SAM3S/SAM3S-EK/sam3s-ek-flash.icf
endif
def-y += -DCHIP_AT91SAM3S -Dsam3s4
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

def-y += -D__AS_BY_PARAI__	 


ifeq ($(board),mingw)
#cflags-y += -Werror
include ../make/mingw.mk
else
$(warning   >> fix your IAR compiler directory.)
IAR_DIR = "D:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0"
include ../make/cortexm3.mk
endif

$(inc-dir)/utility:
	@mkdir -p $@
	
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@
	
dep-mingw:
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include/Std_Types.h Std_Types.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/mcal/Lcd.c Lcd.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/mcal/Lcd.h Lcd.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c main.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/EGL)
	@(echo "  >> prepare link for MINGW done")
	
dep-at91sam3s:
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include/Std_Types.h Std_Types.h)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libboard_sam3s-ek TRUE)
	@(cd $(src-dir);rm -f supc.c syscalls.c board_cstartup_iar.c hsmci_pdc.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/common/core TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/common/cdc TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/device/core TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/usb/device/cdc-serial TRUE)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s/include/trace.h trace.h)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libboard_sam3s-ek/include/led.h led.h)
	@(cd $(inc-dir)/utility;cp -fv $(IAR_DIR)/arm/inc/c/assert.h assert.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m TRUE)
	@(echo "  >> prepare link for AT91SAM3S done")

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board)
	
sg:
	@(cd SgDesign;python ../../../com/python/Sg/Sg.py Sg.xml)	
	
all: exe

clean: clean-obj
	@(echo "  >> clean done ")
	
clean-dist:clean
	@rm -fv $(src-dir)/*
	
	
 

