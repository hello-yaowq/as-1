use-boot?=yes
termux?=no
usepci?=yes
ifeq ($(host),Linux)
tcpip?=lwip
else
tcpip=none
endif
asflags-y += -m32
cflags-y  += -m32 -fno-builtin -fno-stack-protector
ldflags-y += -melf_i386
ifeq ($(use-boot),yes)
link-script = $(src-dir)/linker-app.lds
else
link-script = $(src-dir)/linker.lds
endif
ifeq ($(host), Linux)
COMPILER_PREFIX = 
COMPILER_DIR = /usr
else
COMPILER_PREFIX = 
COMPILER_DIR = C:/MinGW
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
ifeq ($(rtos),tinix)
def-y += -DWITH_PUTS -Dprintf=printk -DTM_PRINTF_BUF_SIZE=128
endif
inc-y += -I/usr/include/newlib
ifeq ($(usepci),yes)
def-y += -DUSE_PCI
endif
ifeq ($(rtos),rtthread)
def-y += -DUSE_OSAL
# heap size 8Mb
def-y += -DRT_HEAP_SIZE=0x800000
endif

ifeq ($(tcpip),lwip)
inc-y += -I$(src-dir)/lwip/src/include
inc-y += -I$(src-dir)/lwip/src/include/ipv4
endif

# heap size 2Mb
def-y += -DconfigTOTAL_HEAP_SIZE=0x200000
def-y += -DPAGE_SIZE=4096 -DconfigTOTAL_PAGE_COUNT=4096
def-y += -D__X86__
#no-gcs=yes
#no-lds=yes
# Entry point of Tinix
# It must be as same as 'KernelEntryPointPhyAddr' in load.inc!!!
#ENTRYPOINT	= 0x30400
#ldflags-y += -s -Ttext $(ENTRYPOINT)

obj-y += $(patsubst %.asm,$(obj-dir)/%.o,$(foreach x,$(dir-y),$(notdir $(wildcard $(addprefix $(x)/*,.asm)))))

ifeq ($(compiler),gcc)
ifeq ($(termux),yes)
COMPILER_PREFIX = 
COMPILER_DIR = /data/data/com.termux/files/usr/
cflags-y =
ldflags-y =
endif
include ../make/gcc.mk
endif

dep-i386: $(obj-dir) $(exe-dir) $(src-dir)/pci.download.done aslwip
	@(mkdir -p $(inc-dir)/arch)
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
	@(cd $(src-dir);$(LNFS) $(ASCORE)/app FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common FALSE)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/common/config FALSE)
	@(cd $(src-dir);sed -e "5c <OsRef name='$(rtos)'/>" infrastructure.xml > $(board)_infrastructure.xml)
	@(cd $(src-dir); rm infrastructure.xml)	
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include FALSE)
	@(cd $(inc-dir);$(LNFS) $(INFRASTRUCTURE)/include/sys)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.bcm2835 TRUE;rm bcm2845.xml *.lds)
	@(cd $(src-dir);$(LNFS) $(APPLICATION)/board.x86 TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.h Os.h)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/kernel/Os.c Os.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/EcuM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/system/SchM TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/diagnostic TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/communication TRUE)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/arch/x86/boot)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/x86/mcal TRUE)
ifeq ($(usepci),yes)
	@(cd $(src-dir);rm -vf Can.c)
else
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
endif
ifeq ($(rtos),tinix)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/arch/x86/bsp TRUE)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/small/os_i.h)
else
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/arch/x86/bsp/pci-x86.c)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/asheap.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/misclib.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/cirq_buffer.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/mbox.c)
ifeq ($(rtos),rtthread)
	@(cd $(inc-dir); $(LNFS) $(download)/rt-thread/include TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/src TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/libcpu/ia32 TRUE)
	@(cd $(src-dir); rm linker-app.lds)
	@(cd $(src-dir); cp -v $(download)/rt-thread/bsp/x86/x86_ram.lds linker-app.lds)
	@(cd $(inc-dir); $(LNFS) $(download)/rt-thread/bsp/x86/drivers/include TRUE)
	@(cd $(src-dir); $(LNFS) $(download)/rt-thread/bsp/x86/drivers/ TRUE)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/rtthread TRUE)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/small/os_i.h)
#	@(cd $(src-dir); sed -i "6c . = 0x400400;" linker-app.lds)
	@(cd $(src-dir); sed -i "37c .bss : { *(.bss) *(.bss.*) }" linker-app.lds)
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
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/misclib.c)
	@(make BSW)
	@(make boot)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for $(board) done")

$(obj-dir)/%.o:%.asm
	(cd $(src-dir);nasm -f elf -o $@ $<)

$(obj-dir)/boot.bin:$(src-dir)/boot/boot.asm
	(cd $(src-dir)/boot;nasm -o $@ $<)

$(obj-dir)/loader.bin:$(src-dir)/boot/loader.asm
	(cd $(src-dir)/boot;nasm -o $@ $<)

$(exe-dir)/TINIX.IMG:
	dd if=/dev/zero of=$@ bs=512 count=2880
	sudo mkfs.vfat $@
	dd conv=notrunc if=$(obj-dir)/boot.bin of=$@ bs=512 count=1
	mkdir -p tmp
	sudo mount -t vfat $@ tmp
	sudo cp $(obj-dir)/loader.bin tmp
	sudo sync
	sudo umount tmp

boot:$(obj-dir)/boot.bin $(obj-dir)/loader.bin $(exe-dir)/TINIX.IMG
