#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2015  AS <parai@foxmail.com>
# *
# * This source code is free software; you can redistribute it and/or modify it
# * under the terms of the GNU General Public License version 2 as published by the
# * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
# *
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# * for more details.
# */
target-y = $(board)

obj-dir = $(CURDIR)/obj/$(board)-$(rtos)-$(compiler)
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src/$(board)-$(rtos)-$(compiler)
inc-dir = $(CURDIR)/src/$(board)-$(rtos)-$(compiler)/include

download = $(CURDIR)/../download

prj-dir = $(subst /release/ascore,,$(CURDIR))

host = $(shell uname)

INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
APPLICATION = $(prj-dir)/com/as.application
VIRTUAL = $(prj-dir)/com/as.virtual
ASCORE = $(prj-dir)/release/ascore
ASCONFIG = $(prj-dir)/com/as.tool/config.infrastructure.system

default: 91 92

# --------------------------------- [ GENERAL SPECIFICATION ] ------------------------------------------- #		 
debug ?= TRUE
ifeq ($(host), Linux)
PY27 = python2
PY34 = python3
else
PY27 = C:/Python27/python
PY34 = C:/Anaconda3/python
endif
LUA   = $(prj-dir)/com/as.tool/lua
LNFS  = $(PY34) $(prj-dir)/release/make/lnfs.py
DEPC  = $(PY34) $(prj-dir)/release/make/dep.py
XCC   = $(PY34) xcc.py
STUDIO = $(PY34) studio.py
SG    = $(PY34) $(prj-dir)/com/as.tool/config.infrastructure.gui/Sg.py
PYS19   = $(PY34) $(prj-dir)/com/as.tool/lua/script/s19.py
#sgapp ?= watch
ifeq ($(host), Linux)
sgapp ?= virtual_cluster
else
sgapp ?= virtual_cluster
endif
# --------------------------------- [ BOARD SPECIFICATION ] ------------------------------------------- #
-include scripts/$(board).mk

def-y += -D__AS_BY_PARAI__	 
def-y += -D$(board)_board
inc-y += -I$(inc-dir)
def-y += -D__use_os_$(rtos)__
ifeq ($(rtos),ucosii)
inc-y += -I$(src-dir)/ucosii
VPATH += $(src-dir)/ucosii
def-y += -DUSE_UCOSII
endif

# --------------------------------- [ COMPILER SPECIFICATION ] ---------------------------------------- #		 

include scripts/common.mk

dep:$(src-dir) $(inc-dir) $(inc-dir)/utility dep-$(board) OS

all:exe
ifeq ($(board),posix)
ifeq ($(host),Linux)
ifeq ($(tcpip),none)
else
	gcc $(src-dir)/socket_ethernet_driver.c -lpthread -DSOCKET_ETHERNET_DRIVER -o $(exe-dir)/eth_driver.exe
endif
endif
endif

depc:
	@($(DEPC) $(obj-dir) $(obj-dir)/as.dep)

clean: 
	@rm -frv ./obj/*
	@rm -fv $(ASCORE)/SgDesign/SgRes/*

# ---------------------------------- [ AT91SAM3S   ] ---------------------------------- #
01:
	make dep board=at91sam3s compiler=cortexm3-icc
02:
	make all board=at91sam3s compiler=cortexm3-icc debug=true
03:
	make all board=at91sam3s compiler=cortexm3-icc debug=false	
# ---------------------------------- [ STM32F107VC ] ---------------------------------- #
11:
	make dep board=stm32f107vc compiler=cortexm3-gcc rtos=toppers_osek
12:
	make all board=stm32f107vc compiler=cortexm3-gcc rtos=toppers_osek debug=true
ifeq ($(use-boot),yes)
	@($(PYS19) -m ../asboot/out/stm32f107vc.s19 out/stm32f107vc.s19 -o out/boot_app.s19)
endif
13:
	make all board=stm32f107vc compiler=cortexm3-gcc rtos=toppers_osek debug=false	

15:
	make dep board=stm32f107vc compiler=cortexm3-icc
16:
	make all board=stm32f107vc compiler=cortexm3-icc debug=true
17:
	make all board=stm32f107vc compiler=cortexm3-icc debug=false

# ---------------------------------- [ lm3s6965evb  ] ---------------------------------- #
21:
	make dep board=lm3s6965evb compiler=cortexm3-gcc rtos=trampoline
22:
	make all board=lm3s6965evb compiler=cortexm3-gcc rtos=trampoline debug=true
23:
	make all board=lm3s6965evb compiler=cortexm3-gcc rtos=toppers_osek debug=false

29:
	@echo "target remote:2345" > ~/.gdbinit
	@echo "file out/lm3s6965evb.exe" >> ~/.gdbinit
	@echo "b main" >> ~/.gdbinit
	@echo "b usage_fault_handler" >> ~/.gdbinit

	@echo "#! /bin/sh" > gdbg.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> gdbg.sh
	@echo "qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial stdio -semihosting -machine lm3s6965evb -kernel out/lm3s6965evb.exe -gdb tcp::2345 -S &" >> gdbg.sh
	@echo "echo target remote:2345 b main c s n" >> gdbg.sh
	@echo "arm-none-eabi-gdb" >> gdbg.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> gdbg.sh
	@echo "echo end of debug" >> gdbg.sh
	@chmod +x gdbg.sh

	@echo "#! /bin/sh" > run.sh
	@echo "pgrep qemu-system-arm|xargs -i kill -9 {}" >> run.sh
	@echo "qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial stdio -semihosting -machine lm3s6965evb -kernel out/lm3s6965evb.exe" >> run.sh
	@echo "echo end of running" >> run.sh
	@chmod +x run.sh

# ---------------------------------- [ s3c2440a    ] ---------------------------------- #
31:
	make dep board=s3c2440a compiler=arm920t-gcc rtos=trampoline
32:
	make all board=s3c2440a compiler=arm920t-gcc rtos=trampoline debug=true
33:
	make all board=s3c2440a compiler=arm920t-gcc rtos=trampoline debug=false	

# ---------------------------------- [ qemu-rpi bcm2835 ] ----------------------------- #
41:
	make dep board=bcm2835 compiler=gcc rtos=trampoline
42:
	make all board=bcm2835 compiler=gcc rtos=trampoline debug=true

$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz:
	@wget https://github.com/idrawone/qemu-rpi/raw/master/tools/qemu-system-arm-rpi_UART1.tar.gz
	@tar xf $@

qemuparams?="-nographic"
qemu-rpi:$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz
#	@(./qemu-system-arm -cpu arm1176 -m 128 -M raspi ${qemuparams} -kernel out/bcm2835.exe -s -S)
	@(./qemu-system-arm -cpu arm1176 -m 128 -M raspi ${qemuparams} -kernel out/bcm2835.exe -serial tcp:127.0.0.1:1103,server)
# ---------------------------------- [ versatilepb ] ----------------------------- #
51:
	make dep board=versatilepb compiler=gcc rtos=trampoline
52:
	make all board=versatilepb compiler=gcc rtos=trampoline debug=true

53:
	make dep board=versatilepb compiler=gcc rtos=askar
54:
	make all board=versatilepb compiler=gcc rtos=askar debug=true
qemu-ver: floppy.img
	@($(download)/qemu/arm-softmmu/qemu-system-arm -m 128 -M versatilepb ${qemuparams} -kernel out/versatilepb.exe -serial tcp:127.0.0.1:1103,server  \
		-drive file=floppy.img,if=virtio,format=raw -show-cursor -usb -usbdevice tablet -device virtio-rng-pci \
		-device pci-ascan -device pci-asnet -device pci-asblk )
#	@(qemu-system-arm -m 128 -M versatilepb ${qemuparams} -kernel out/versatilepb.exe -serial stdio -gdb tcp::1234 -S)

# ---------------------------------- [ i386 ] ----------------------------- #
61:
	make dep board=i386 compiler=gcc rtos=rtthread
floppy.img:
	wget https://github.com/bajdcc/tinix/raw/master/floppy.img

62: floppy.img
	make all board=i386 compiler=gcc rtos=rtthread debug=true
	mkdir -p tmp
	sudo mount -t vfat out/TINIX.IMG tmp -o loop
	sudo cp out/i386.exe tmp/kernel.bin && sync
	sudo umount tmp
	sudo mount -t vfat floppy.img tmp -o loop && sync
	sudo cp out/i386.exe tmp/boot/oskernel
	sudo umount tmp

qemu-i386:
	qemu-system-i386 -fda out/TINIX.IMG -boot a -m 64M

qemu-i386-grub:
	$(download)/qemu/i386-softmmu/qemu-system-i386 -fda floppy.img -boot a -m 64M -serial stdio \
		-drive file=floppy.img,if=virtio,format=raw -show-cursor -usb -usbdevice tablet -device virtio-rng-pci \
		-device pci-ascan -device pci-asnet -device pci-asblk \

# ---------------------------------- [ posix       ] ---------------------------------- #
91:
	make dep board=posix compiler=posix-gcc gui=GTK rtos=smallos
92:
	make all board=posix compiler=posix-gcc gui=GTK rtos=smallos debug=true
ifeq ($(host),Linux)
	pgrep viper|xargs -i kill -9 {}
else
	gcc -O2 -shared -Wall -o $(exe-dir)/backtrace.dll $(INFRASTRUCTURE)/diagnostic/Det/backtrace.c -lbfd -lintl -liberty -limagehlp -D__ASDEBUG_BACKTRACE__
	@cp -v $(exe-dir)/backtrace.dll C:/MinGW/bin
endif
93:
	make all board=posix compiler=posix-gcc gui=GTK rtos=toppers_osek debug=false	
94:
	make dll board=posix compiler=posix-gcc gui=GTK rtos=toppers_osek debug=true	
	@cp -v ./out/posix.dll ../asboot/out/core.dll


