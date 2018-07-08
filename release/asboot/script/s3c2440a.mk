link-script = $(src-dir)/linker-boot.lds
def-y += -DCHIP_S3C2440A
def-y += -DUSE_MCU -DUSE_SCHM -DUSE_ECUM -DUSE_KERNEL
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PDUR -DUSE_CANTP -DUSE_DCM
def-y += -DTEXT_BASE=0x30000000

ifeq ($(compiler),arm920t-gcc)
ifeq ($(host), Linux)
#COMPILER_PREFIX = arm-linux-gnueabi-
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
ldflags-y += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
ldflags-y += -lc -lgcc -L$(COMPILER_DIR)/arm-none-eabi/lib -L$(COMPILER_DIR)/lib/gcc/arm-none-eabi/4.8.3
#COMPILER_PREFIX = arm-linux-gnueabihf-
#COMPILER_DIR = C:/opt/Linaro/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04
endif
include ../make/arm920t.gcc.mk
endif

dep-s3c2440a:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/tinyos FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/arm920t/gcc/startup.S)
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/s3c2440a/mcal/Can.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/s3c2440a/mcal/s3c2440.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/s3c2440a/mcal/Mcu.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/script TRUE)	
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make OS)
	@(make BSW)
	@(echo "  >> prepare link for s3c2440a done")		
