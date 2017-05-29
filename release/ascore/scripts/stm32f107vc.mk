
use-boot?=yes
tcpip=lwip
ifeq ($(compiler),cortexm3-gcc)
ifeq ($(use-boot),yes)
link-script = $(src-dir)/linker-app.lds
else
link-script = $(src-dir)/linker-common.lds
endif
endif
ifeq ($(compiler),cortexm3-icc)
link-script = $(COMPILER_DIR)/arm/config/linker/ST/stm32f107xC.icf
endif
def-y += -DCHIP_STM32F10X -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER
def-y += -DUSE_KERNEL -DUSE_PORT -DUSE_MCU -DUSE_DIO -DUSE_ECUM -DUSE_SCHM
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM -DUSE_OSEKNM -DUSE_XCP
#def-y += -DUSE_DET
def-y += -DUSE_SOAD
def-y += -DUSE_SIMUL_CAN
inc-y += -I$(src-dir)/lwip/src/include
inc-y += -I$(src-dir)/lwip/src/include/ipv4

include scripts/cortex_m3.mk

dep-stm32f107vc:aslwip
	@(mkdir -p $(inc-dir)/arch)
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);rm -f widget_refresh.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
ifeq ($(tcpip),lwip)
	@(cd $(src-dir);rm -f lwip_timers.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/core TRUE)
	@(cd $(src-dir);rm icmp6.c inet6.c ip6_addr.c ip6.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/netif TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/api TRUE)
	@(cd $(src-dir);rm ethernetif.c;mv timers.c lwip_timers.c)
	@(cd $(inc-dir)/arch;$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/lwip/arch TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/lwip TRUE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/Crc TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32_ETH_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/STM32F10x_StdPeriph_Driver TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x FALSE)
ifeq ($(rtos),smallos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/small/portable/armv7_m/gcc FALSE)
else
	$(error for small os, non-gcc compiler is not supported)
endif
endif
ifeq ($(rtos),freertos)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/include FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/MemMang FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/GCC/ARM_CM3 FALSE)
endif
ifeq ($(compiler),cortexm3-icc)	
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/source/portable/IAR/ARM_CM3 FALSE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/freertos/config TRUE)
	@(cd $(src-dir);rm -f heap_1.c heap_2.c heap_3.c heap_5.c)
endif
ifeq ($(rtos),toppers_osek)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/gcc FALSE)
endif
ifeq ($(compiler),cortexm3-icc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers_osek/portable/armv7_m/icc FALSE)
endif
endif
ifeq ($(rtos),atk2-sc4)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/armv7_m FALSE)
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/toppers-atk/atk2-sc4/portable/armv7_m/gcc TRUE)
else
	$(error for atk2-sc4 os, non-gcc compiler is not supported)
endif
endif
ifeq ($(compiler),cortexm3-gcc)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib TRUE)
endif
	@(cd $(inc-dir);$(LNFS) $(CMSIS))
	@(cp -fv $(src-dir)/rom.gdbinit $(HOME)/.gdbinit)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(make BSW)
	@(cd $(src-dir);rm -fv Can_Cfg.c Can_Cfg.h Can_Lcfg.c Can_PBCfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Lcfg.c)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board)/common/Can_Cfg.h)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for STM32F107VC done")
