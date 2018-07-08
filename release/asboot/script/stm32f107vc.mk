
include script/cortex_m3.mk



ifeq ($(board),stm32f107vc)
ifeq ($(compiler),cortexm3-gcc)
link-script = $(src-dir)/linker-boot.lds
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_MCU -DUSE_SCHM -DUSE_ECUM -DUSE_KERNEL
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PDUR -DUSE_CANTP -DUSE_DCM
ifeq ($(led_app),yes)
def-y += -DUSE_DIO -DUSE_PORT
endif
#def-y += -DUSE_DET
def-y += -DUSE_SIMUL_CAN
endif


dep-stm32f107vc: 
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/tinyos FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/tinyos/portable/armv7_m FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/tinyos/portable/armv7_m/gcc FALSE)
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Flash.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Can.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Mcu.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Mcu_ConfigTypes.h)
ifeq ($(led_app),yes)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/leds TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Dio.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Port.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Dio_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Dio_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_Cfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_Cfg.h)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Port_ConfigTypes.h)
endif
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/script TRUE)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make OS)
	@(make BSW)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Lcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Cfg.h)
	@(echo "  >> prepare link for STM32F107VC done")		
