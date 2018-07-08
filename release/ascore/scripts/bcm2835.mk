use-boot?=yes
asflasg-y += -mcpu=arm1176jzf-s -fpic
cflags-y  += -mcpu=arm1176jzf-s -fpic
ifeq ($(use-boot),yes)
link-script = $(src-dir)/linker-app.lds
else
link-script = $(src-dir)/linker.lds
endif
ifeq ($(host), Linux)
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = /usr
ldflags-y += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
else
COMPILER_PREFIX = arm-none-eabi-
COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
ldflags-y += -lc -lgcc -L$(COMPILER_DIR)/arm-none-eabi/lib -L$(COMPILER_DIR)/lib/gcc/arm-none-eabi/4.8.3
endif
ifeq ($(rtos),trampoline)
def-y += -DWITH_SYSTEM_CALL=YES
def-y += -DWITH_DEBUG=YES
endif
def-y += -DUSE_KERNEL -DUSE_ECUM -DUSE_SCHM -DUSE_MCU
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM -DUSE_OSEKNM -DUSE_XCP
def-y += -DUSE_DET

ifeq ($(compiler),gcc)
cflags-y += -mstructure-size-boundary=8
include ../make/gcc.mk
endif

dep-bcm2835:
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/bcm2835/bsp TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/bcm2835/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
ifeq ($(rtos),ucosii)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core ucosii)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core/ucos_ii.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/core/os_dbg_r.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/tinyos/os_i.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii FALSE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/ucos_ii/portable/raspi/gnu TRUE)
endif
ifeq ($(rtos),trampoline)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/os TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/debug TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/machines/cortex-a TRUE)
	@(cd $(src-dir);rm tpl_os_stm_kernel.c)
endif
	@(make BSW)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for bcm2835 done")
