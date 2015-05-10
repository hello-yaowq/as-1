# specific target as libcan.a
# slect GUI openVG/GTK
gui-used = GTK

#target-y = FreeRTOS
target-y = toppers_osek
# this is only for make dep
# AT91SAM3S/MINGW
arch-y = AT91SAM3S

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src

LNFS  = python ../../make/lnfs.py
		 
ldflags-y += -lm -lwinmm
	
	 
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0` \
			 -lpthread

ifeq ($(gui-used),GTK)			 
def-y += -DGUI_USE_GTK			 
else	 
def-y += -DGUI_USE_OPENVG	
endif		 
			 
ldflags-y += -lstdc++	 
			 			 
dir-y += ./SgDesign/SgRes		
dir-y += ./app
dir-y += ./app/config
inc-y += -I./include
inc-y += -I./include/EGL
inc-y += -I./include/VG
inc-y += -I./app/config
inc-dir = ./include

def-y += -DOPENVG_STATIC_LIBRARY -D_WIN32 -DEGL_STATIC_LIBRARY

def-y += -D__AS_BY_PARAI__



#cflags-y += -Werror

include ../make/Makefile
$(inc-dir):
	@mkdir -p $@
$(src-dir):
	@mkdir -p $@
dep:$(src-dir) $(inc-dir)
ifeq ($(arch-y),AT91SAM3S)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/arch/at91sam3s/libraries)
else
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/Lcd.c Lcd.c)
endif	
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Lcd.h Lcd.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Std_Types.h Std_Types.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/SgDraw.c SgDraw.c)
ifeq ($(target-y),FreeRTOS)	
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/FreeRTOS/Source TRUE)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/FreeRTOS/Config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c main.c)
else
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/toppers_osek/kernel TRUE)
ifeq ($(arch-y),AT91SAM3S)	
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/toppers_osek/portable/armv7_m TRUE)
else
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/toppers_osek/portable/mingw TRUE)
endif	
endif
ifeq ($(gui-used),openVG)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/cplusplus/vg FALSE)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/cplusplus/vg/win32 FALSE)
	@(cd $(inc-dir);$(LNFS) ../../../com/clang/include/VG)
	@(cd $(inc-dir);$(LNFS) ../../../com/clang/include/EGL)
endif
	
	
sg:
	@(cd SgDesign;python ../../../com/python/Sg/Sg.py Sg.xml)	
all: exe

clean: clean-obj
	@rm -fv $(src-dir)/*
	
	
 

