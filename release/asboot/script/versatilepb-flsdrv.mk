asflasg-y += -mcpu=arm926ej-s -marm -fpic
cflags-y  += -mcpu=arm926ej-s -marm -fpic
link-script = $(src-dir)/linker-flsdrv.lds
ifeq ($(host), Linux)
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
ldflags-y += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
ldflags-y += -lc -lgcc -L$(COMPILER_DIR)/arm-none-eabi/lib -L$(COMPILER_DIR)/lib/gcc/arm-none-eabi/4.8.3
endif

def-y += -DFLASH_DRIVER_DYNAMIC_DOWNLOAD
# chcck the linker-app.lds
def-y += -DFLS_START_ADDRESS=0x00040000
def-y += -DFLS_END_ADDRESS=0x08000000
ifeq ($(compiler),gcc)
cflags-y += -mstructure-size-boundary=8
include ../make/gcc.mk
endif

dep-versatilepb-flsdrv: 
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/RamFlash.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.bcm2835/script TRUE)

