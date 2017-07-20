asflasg-y += -mcpu=arm926ej-s -marm -fpic
cflags-y  += -mcpu=arm926ej-s -marm -fpic
link-script = $(src-dir)/linker-boot.lds
ifeq ($(host), Linux)
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
ldflags-y += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
ldflags-y += -lc -lgcc -L$(COMPILER_DIR)/arm-none-eabi/lib -L$(COMPILER_DIR)/lib/gcc/arm-none-eabi/4.8.3
endif

def-y += -DUSE_MCU -DUSE_SCHM -DUSE_ECUM -DUSE_KERNEL
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PDUR -DUSE_CANTP -DUSE_DCM
# define __LINUX__ if want the Flash Driver to be builtin
# def-y += -D__LINUX__
# check the linker-app.lds
def-y += -DFLS_START_ADDRESS=0x00040000
def-y += -DFLS_END_ADDRESS=0x08000000
ifeq ($(compiler),gcc)
cflags-y += -mstructure-size-boundary=8
include ../make/gcc.mk
endif
# 128MB
def-y += -DMEMORY_SIZE=0x8000000
def-y += -DSYSTEM_REGION_START=0x10000000
def-y += -DSYSTEM_REGION_END=0x101f4000
def-y += -DPAGE_SIZE=0x1000

dep-versatilepb:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/portable/raspi/gnu/startup.s startup.S)
	@(cd $(src-dir);echo "" > portable.h)
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/versatilepb/bsp TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/versatilepb/mcal/Can.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/RamFlash.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/versatilepb/mcal/Mcu.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_ConfigTypes.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.bcm2835/script TRUE)	
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make OS)
	@(make BSW)
	@(echo "  >> prepare link for VERSATILEPB done")		
