use-boot?=yes
termux?=no
asflasg-y +=
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
def-y += -DUSE_OSAL
def-y += -DWITH_PUTS -Dprintf=printk -DTM_PRINTF_BUF_SIZE=128

inc-y += -I/usr/include/newlib

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

dep-i386: $(obj-dir) $(exe-dir)
	@(cd $(src-dir);$(LNFS) $(ASCONFIG))
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
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/arch/x86/bsp TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/x86/mcal TRUE)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/arch/common/mcal/SCan.c)
ifeq ($(rtos),tinix)
	@(cd $(src-dir); $(LNFS) $(INFRASTRUCTURE)/system/kernel/small/os_i.h)
endif
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/stdio_printf.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/clib/misclib.c)
	@(make BSW)
	@(make boot)
	@(cd  $(src-dir); rm -fv Dem* J1939Tp* Lin* NvM* Fee* Ea* Eep* Fls* RPmsg* Ipc* VirtQ* Rproc*)
	@(echo "  >> prepare link for $(board) done")

$(obj-dir)/%.o:%.asm
	nasm -f elf -o $@ $<

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
	sudo umount tmp

boot:$(obj-dir)/boot.bin $(obj-dir)/loader.bin $(exe-dir)/TINIX.IMG
