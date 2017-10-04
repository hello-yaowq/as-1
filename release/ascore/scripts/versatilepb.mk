use-boot?=no
usepci?=yes
termux?=no
ifeq ($(host),Linux)
tcpip?=lwip
else
tcpip=none
endif
export asflasg-y += -mcpu=arm926ej-s -marm -fpic
export cflags-y  += -mcpu=arm926ej-s -marm -fpic
ifeq ($(use-boot),yes)
link-script = $(src-dir)/linker-app.lds
else
link-script = $(src-dir)/linker.lds
endif
ifeq ($(host), Linux)
export COMPILER_PREFIX = arm-none-eabi-
export COMPILER_DIR = /usr
ldflags-y += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
else
export COMPILER_PREFIX = arm-none-eabi-
export COMPILER_DIR = C:/gcc-arm-none-eabi-4_8-2014q1-20140314-win32
ldflags-y += -lc -lgcc -L$(COMPILER_DIR)/arm-none-eabi/lib -L$(COMPILER_DIR)/lib/gcc/arm-none-eabi/4.8.3
endif
ifeq ($(rtos),trampoline)
def-y += -DWITH_SYSTEM_CALL=YES
#def-y += -DWITH_DEBUG=YES
endif
def-y += -DUSE_KERNEL -DUSE_ECUM -DUSE_SCHM -DUSE_MCU
# network of CAN
def-y += -DUSE_CAN -DUSE_CANIF -DUSE_PRUR -DUSE_COM -DUSE_COMM -DUSE_CANTP -DUSE_CANNM	\
		 -DUSE_DCM -DUSE_CANNM -DUSE_CANSM -DUSE_PDUR -DUSE_NM -DUSE_OSEKNM -DUSE_XCP
def-y += -DUSE_DET
ifeq ($(tcpip),lwip)
def-y += -DUSE_SOAD
def-y += -DUSE_LWIP
endif
# 128MB
def-y += -DMEMORY_SIZE=0x8000000
def-y += -DSYSTEM_REGION_START=0x10000000
def-y += -DSYSTEM_REGION_END=0x101f4000
def-y += -DPAGE_SIZE=0x1000
ifeq ($(usepci),yes)
def-y += -DUSE_PCI
ifeq ($(EMAIL),parai@foxmail.com)
# on Travis CI, download ff13 always failed
else
def-y += -DUSE_FATFS
inc-y += -I$(download)/ff13/source
ldflags-y += -L$(INFRASTRUCTURE)/system/fs/out -lff13-$(board)
def-y += -DCONFIG_USE_DEFAULT_CFG=1
inc-y += -I$(download)/lwext4/include
#ldflags-y += -llwext4-$(board)
#def-y += -DUSE_LWEXT4
endif
endif
ifeq ($(rtos),rtthread)
def-y += -DUSE_OSAL
# heap size 2Mb
def-y += -DRT_HEAP_SIZE=0x200000
def-y += -DIDLE_THREAD_STACK_SIZE=4096
endif
def-y += -DconfigTOTAL_HEAP_SIZE=0x200000
# heap size 2Mb
def-y += -DconfigTOTAL_PAGE_COUNT=4096

ifeq ($(tcpip),lwip)
inc-y += -I$(src-dir)/lwip/src/include
inc-y += -I$(src-dir)/lwip/src/include/ipv4
endif

ifeq ($(compiler),gcc)
cflags-y += -mstructure-size-boundary=8
ifeq ($(termux),yes)
COMPILER_PREFIX = 
COMPILER_DIR = /data/data/com.termux/files/usr/
cflags-y =
ldflags-y =
endif
include ../make/gcc.mk
endif

dep-versatilepb: $(download)/rt-thread $(src-dir)/pci.download.done aslwip aslibfatfs
	@(mkdir -p $(inc-dir)/arch)
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.bcm2835 TRUE;rm bcm2845.xml)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.$(board) TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/bcm2835/bsp/linker.lds)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/$(board)/bsp TRUE)
	@(cd $(src-dir);mv pci.c pci-versatilepb.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/$(board)/mcal TRUE)
ifeq ($(usepci),yes)
	@(cd $(src-dir);rm -vf Can.c)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/asheap.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/cirq_buffer.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/mbox.c)
ifeq ($(rtos),askar)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/askar/kernel TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/include TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/portable/arm TRUE)
endif
ifeq ($(rtos),trampoline)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/os TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/debug TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/autosar TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/trampoline/machines/cortex-a TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/asheap.c)
	@(cd $(src-dir);rm tpl_os_stm_kernel.c)
endif
ifeq ($(rtos),rtthread)
	@(cd $(inc-dir); $(LNFS) $(download)/rt-thread/include TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/src TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/src/irq.c rt_irq.c)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/src/timer.c rt_timer.c)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/libcpu/arm/arm926 TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/bsp/asm9260t/platform/rt_low_level_gcc.inc)
	@(cd $(src-dir); rm -f *_rvds.S *_iar.S)
	@(cd $(src-dir); cp $(INFRASTRUCTURE)/system/kernel/trampoline/machines/cortex-a/tpl_startup.S startup.S)
	@(cd $(src-dir); head start_gcc.S -n 58 | tail -n 10 >> startup.S);
	@(cd $(src-dir); tail start_gcc.S -n 50 >> startup.S)
	@(cd $(src-dir); sed -i "3c #" startup.S)
	@(cd $(src-dir); sed -i "32c irq_handler: .word IRQ_Handler" startup.S)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/rtthread TRUE)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/small/os_i.h)
#	@(cd $(src-dir); rm *.lds -f)
#	@(cd $(src-dir); cp $(download)/rt-thread/bsp/asm9260t/link_scripts/sdram.ld linker-app.lds -fv)
#	@(cd $(src-dir); sed -i "6c . = 0x8000;" linker-app.lds)
	@(cd $(src-dir); rm trap.c start_gcc.S)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/components/finsh TRUE)
endif
ifeq ($(tcpip),lwip)
	@(cd $(src-dir);rm -f lwip_timers.c timers.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/core TRUE)
ifeq ($(rtos),rtthread)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/core/mem.c lwip_mem.c)
endif
	@(cd $(src-dir);rm icmp6.c inet6.c ip6_addr.c ip6.c)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/netif TRUE)
	@(cd $(src-dir);$(LNFS) $(download)/lwip/src/api TRUE)
	@(cd $(src-dir);rm ethernetif.c;mv timers.c lwip_timers.c)
	@(cd $(inc-dir)/arch;$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/lwip/arch TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/lwip/netbios.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/stm32f1/lwip/lwipopts.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/lwip/sys_arch.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/posix/lwip/ethernetif.h)
	@(cd $(src-dir);$(LNFS) $(download)/lwip-contrib/apps/httpserver TRUE)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/stdio_printf.c)
	@(make BSW)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for $(board) done")
