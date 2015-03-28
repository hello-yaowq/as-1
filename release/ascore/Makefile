# specific target as libcan.a
target-y = FreeRTOS

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src

LNFS  = python ../../make/lnfs.py
		 
ldflags-y += -lm -lwinmm
		 
cflags-y  += `pkg-config --cflags gtk+-3.0`
ldflags-y += `pkg-config --cflags gtk+-3.0` \
			 `pkg-config --libs gtk+-3.0 glib-2.0 gthread-2.0` \
			 -lpthread
			 
dir-y += ./SgDesign/SgRes		 

include ../make/Makefile

$(src-dir):
	@mkdir -p $@
dep:$(src-dir)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/Lcd.c Lcd.c)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Lcd.h Lcd.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Std_Types.h Std_Types.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/Sg.h Sg.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/include/SgDraw.h SgDraw.h)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/Sg.c Sg.c)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/SgDraw.c SgDraw.c)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/FreeRTOS/Source Source)
	@(cd $(src-dir);$(LNFS) ../../../com/clang/c/FreeRTOS/Config Config)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_4.c heap_5.c)
	
sg:
	@(cd SgDesign;python ../../../com/python/Sg/Sg.py Sg.xml)	
all: exe

clean: clean-obj
	@rm -frv ./$(src-dir)
	
	
 

