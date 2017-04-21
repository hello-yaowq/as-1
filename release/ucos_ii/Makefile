#/**
# * AS - the open source Automotive Software on https://github.com/parai
# *
# * Copyright (C) 2016  AS <parai@foxmail.com>
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

# CPU architecture setting: win32 posix cm3 raspi ...
ifeq ($(shell uname), Linux)
export ARCH ?= raspi
else
export ARCH ?= win32
endif

TARGET = ucOS_II_Demo

obj-dir = $(CURDIR)/objs
out-dir = $(CURDIR)/out

COM = $(CURDIR)/../../com
KERNEL = ${COM}/as.infrastructure/system/kernel/ucos_ii

# default compilers
ifeq (${ARCH}, posix)
CC = gcc
LD = gcc
endif

ifeq (${ARCH}, win32)
CC = gcc
LD = gcc
endif

ifeq (${ARCH}, cm3)
export CROSS_COMPILE ?= arm-none-eabi-
# board select: stm32f107vc at91sam3s
export BOARD ?= stm32f107vc
AS = ${CROSS_COMPILE}as
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
endif

ifeq (${ARCH}, raspi)
export CROSS_COMPILE ?= arm-none-eabi-
AS = ${CROSS_COMPILE}as
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
endif

# make verbose or not
export V ?= 0
ifeq ($(V),1)
Q=
else
Q=@
endif

# default FLAGS
CFLAGS += -O0 -g -D__arch_${ARCH}__ -std=gnu99
LDFLAGS += -O0 -g
ifeq (${ARCH}, posix)
LDFLAGS += -lpthread
LDFLAGS += -Wl,-Map,${out-dir}/${TARGET}.map
endif
ifeq (${ARCH}, win32)
LDFLAGS += -lwinmm
LDFLAGS += -Wl,-Map,${out-dir}/${TARGET}.map
endif

ifeq (${ARCH}, cm3)
ASFLAGS += -mcpu=cortex-m3  -mthumb
CFLAGS  += -mcpu=cortex-m3  -mthumb
LDFLAGS += -T ${COM}/as.application/board.${BOARD}/script/linker-app.lds
LDFLAGS += -Map ${out-dir}/${TARGET}.map
endif

ifeq (${ARCH}, raspi)
ASFLAGS += -mcpu=arm1176jzf-s -fpic -ffreestanding
CFLAGS  += -mcpu=arm1176jzf-s -fpic -ffreestanding
LDFLAGS += -lc -lgcc -L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/4.8.2
LDFLAGS += -T ${COM}/as.infrastructure/arch/bcm2835/bsp/linker.lds
LDFLAGS += -Map ${out-dir}/${TARGET}.map
endif

ifeq (${ARCH}, posix)
VPATH += ${KERNEL}/portable/posix
CFLAGS += -I${KERNEL}/portable/posix
obj-y += ${obj-dir}/os_cpu_c.o
endif

ifeq (${ARCH}, win32)
VPATH += ${KERNEL}/portable/win32
CFLAGS += -I${KERNEL}/portable/win32
obj-y += ${obj-dir}/os_cpu_c.o ${obj-dir}/os_trace.o 
endif

ifeq (${ARCH}, cm3)
VPATH += ${KERNEL}/portable/cortex-m3/gnu
CFLAGS += -I${KERNEL}/portable/cortex-m3/gnu
obj-y += ${obj-dir}/os_cpu_c.o ${obj-dir}/os_cpu_a.o ${obj-dir}/startup.o
endif

ifeq (${ARCH}, raspi)
VPATH += ${KERNEL}/portable/raspi/gnu ${COM}/as.infrastructure/arch/bcm2835/bsp
CFLAGS += -I${KERNEL}/portable/raspi/gnu -I${COM}/as.infrastructure/arch/bcm2835/bsp
obj-y += ${obj-dir}/os_cpu_c.o ${obj-dir}/os_cpu_a.o ${obj-dir}/startup.o
obj-y += ${obj-dir}/interrupts.o ${obj-dir}/syscalls.o ${obj-dir}/timer.o ${obj-dir}/uart.o
endif

VPATH += ${KERNEL}/core
CFLAGS += -I${KERNEL}/core
obj-y += ${obj-dir}/ucos_ii.o

VPATH += ${CURDIR}/demo
CFLAGS += -I${CURDIR}/demo
obj-y += ${obj-dir}/main.o
ifeq (${ARCH}, cm3)
obj-y += ${obj-dir}/apphooks.o
endif

default:all

$(obj-dir)/%.o:%.asm
	@echo
	@echo "  >> AS $(notdir $<)"	
	${Q} $(AS) $(ASFLAGS) -o $@ -c $<

$(obj-dir)/%.o:%.s
	@echo
	@echo "  >> AS $(notdir $<)"	
	${Q} $(CC) $(ASFLAGS) -o $@ -c $<

$(obj-dir)/%.o:%.c
	@echo
	@echo "  >> CC $(notdir $<)"
	@$(CC) $(CFLAGS) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ $<
	${Q} $(CC) $(CFLAGS) -o $@ -c $<

${obj-dir}:
	mkdir -p $@

${out-dir}:
	mkdir -p $@

$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz:
	@wget https://github.com/idrawone/qemu-rpi/raw/master/tools/qemu-system-arm-rpi_UART1.tar.gz
	@tar xf $@

qemuparams?="-nographic"
qemu-rpi:$(CURDIR)/qemu-system-arm-rpi_UART1.tar.gz
	@(./qemu-system-arm -cpu arm1176 -m 128 -M raspi ${qemuparams} -kernel $(out-dir)/$(TARGET))

all: ${obj-dir} ${out-dir} ${obj-y}
	@echo "  >> LD $(TARGET)"
	${Q} $(LD) $(obj-y) $(LDFLAGS) -o $(out-dir)/$(TARGET)

clean:
	@rm -frv ${obj-dir} ${out-dir}
