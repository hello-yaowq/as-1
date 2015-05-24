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
inc-y += -I./src/libraries/libchip_sam3s
inc-y += -I./src/libraries/libchip_sam3s/include
inc-y += -I./src/libraries/libboard_sam3s-ek
inc-y += -I./src/libraries/libboard_sam3s-ek/include
VPATH += ./src/libraries/libchip_sam3s/source
VPATH += ./src/libraries/libboard_sam3s-ek/source
inc-y += -I./src/libraries
dir-y += ./src/libraries/usb/common/core
dir-y += ./src/libraries/usb/common/cdc
dir-y += ./src/libraries/usb/device/core
dir-y += ./src/libraries/usb/device/cdc-serial
obj-y +=$(obj-dir)/portableS.o $(obj-dir)/startup.o 
obj-y +=$(obj-dir)/ads7843.o \
        $(obj-dir)/at45d.o \
        $(obj-dir)/at45_spi.o \
        $(obj-dir)/bmp.o \
        $(obj-dir)/board_lowlevel.o \
        $(obj-dir)/board_memories.o \
        $(obj-dir)/clock.o \
        $(obj-dir)/frame_buffer.o \
        $(obj-dir)/hamming.o \
        $(obj-dir)/ili9325.o \
        $(obj-dir)/iso7816_4.o \
        $(obj-dir)/lcdd.o \
        $(obj-dir)/lcd_draw.o \
        $(obj-dir)/lcd_font.o \
        $(obj-dir)/lcd_font10x14.o \
        $(obj-dir)/led.o \
        $(obj-dir)/math.o \
        $(obj-dir)/rand.o \
        $(obj-dir)/timetick.o \
        $(obj-dir)/trace.o \
        $(obj-dir)/tsd_ads7843.o \
        $(obj-dir)/tsd_com.o \
        $(obj-dir)/uart_console.o \
        $(obj-dir)/wav.o \
        $(obj-dir)/wm8731.o \
        $(obj-dir)/acc.o \
        $(obj-dir)/adc.o \
        $(obj-dir)/async.o \
        $(obj-dir)/crccu.o \
        $(obj-dir)/dacc.o \
        $(obj-dir)/efc.o \
        $(obj-dir)/exceptions.o \
        $(obj-dir)/flashd.o \
        $(obj-dir)/hsmci.o \
        $(obj-dir)/pio.o \
        $(obj-dir)/pio_capture.o \
        $(obj-dir)/pio_it.o \
        $(obj-dir)/pmc.o \
        $(obj-dir)/pwmc.o \
        $(obj-dir)/rtc.o \
        $(obj-dir)/rtt.o \
        $(obj-dir)/spi.o \
        $(obj-dir)/spi_pdc.o \
        $(obj-dir)/ssc.o \
        $(obj-dir)/tc.o \
        $(obj-dir)/twi.o \
        $(obj-dir)/twid.o \
        $(obj-dir)/usart.o \
        $(obj-dir)/wdt.o
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
	
dep:$(src-dir) $(inc-dir) $(inc-dir)/utility
ifeq ($(board),at91sam3s)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libchip_sam3s/include/trace.h trace.h)
	@(cd $(inc-dir)/utility;$(LNFS) $(INFRASTRUCTURE)/arch/at91sam3s/libraries/libboard_sam3s-ek/include/led.h led.h)
	@(cd $(inc-dir)/utility;cp -fv $(IAR_DIR)/arm/inc/c/assert.h assert.h)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/mcal/Lcd.c Lcd.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/mcal/Lcd.h Lcd.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/SgDraw.c SgDraw.c)	
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include/Std_Types.h Std_Types.h)
ifeq ($(board),mingw)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c main.c)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
ifeq ($(board),at91sam3s)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m TRUE)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/mingw TRUE)
endif	
endif
ifeq ($(gui-used),openVG)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/mingw/openvg/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/include/VG)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/gui/openvg/EGL)
endif
#	@(gfind $(src-dir)/libraries -name "*.[cC]" > clist.txt)
	
	
sg:
	@(cd SgDesign;python ../../../com/python/Sg/Sg.py Sg.xml)	
	
all: exe
	@(echo enjoy yourself)
clean: clean-obj
	
clean-dist:clean
	@rm -fv $(src-dir)/*
	
	
 

