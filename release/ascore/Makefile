# specific target as libcan.a
target-y = FreeRTOS

obj-dir = ./obj
exe-dir = ./out
src-dir = ./Source

dir-y += ./Config

obj-y += $(obj-dir)/port.o		\
		 $(obj-dir)/heap_3.o

VPATH += ./Source/portable/MemMang	\
		 ./Source/portable/MSVC-MingW

inc-y += -I./Source/include		 
inc-y += -I./Source/portable/MemMang	\
		 -I./Source/portable/MSVC-MingW
		 
ldflags-y += -lm -lwinmm		 

include ../make/Makefile

dep:
	@(ln -fs ../../com/clang/c/FreeRTOS/Source Source)
	@(ln -fs ../../com/clang/c/FreeRTOS/Config Config)
	
all: exe

clean: clean-obj
	@rm -frv ./Source
	@rm -frv ./Config
	
	
 

