
include script/cortex_m3.mk

ifeq ($(board),stm32f107vc-flsdrv)
ifeq ($(compiler),cortexm3-gcc)
link-script = $(src-dir)/linker-flsdrv.lds
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DFLASH_DRIVER_DYNAMIC_DOWNLOAD
endif


dep-stm32f107vc-flsdrv: 
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal/Flash.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.stm32f107vc/script TRUE)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
