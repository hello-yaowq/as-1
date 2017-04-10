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

TARGET = ucOS_II_Demo

obj-dir = $(CURDIR)/objs
out-dir = $(CURDIR)/out

KERNEL = $(shell readlink -f  $(CURDIR)/../../com/as.infrastructure/system/kernel/ucos_ii)

# default compilers
CC = gcc
LD = gcc

# make verbose or not
export V ?= 0
ifeq ($(V),1)
Q=@
else
Q=
endif

# default FLAGS
CFLAGS += -O0 -g
LDFLAGS += -O0 -g -lwinmm


default:all

VPATH += ${KERNEL}/portable/win32
CFLAGS += -I${KERNEL}/portable/win32
obj-y += ${obj-dir}/os_cpu_c.o ${obj-dir}/os_trace.o 

VPATH += ${KERNEL}/core
CFLAGS += -I${KERNEL}/core
obj-y += ${obj-dir}/ucos_ii.o

VPATH += ${CURDIR}/demo
CFLAGS += -I${CURDIR}/demo
obj-y += ${obj-dir}/main.o

$(obj-dir)/%.o:%.c
	@echo
	@echo "  >> CC $(notdir $<)"
	@$(CC) $(CFLAGS) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ $<
	${Q} $(CC) $(CFLAGS) -o $@ -c $<

${obj-dir}:
	mkdir -p $@

${out-dir}:
	mkdir -p $@

all: ${obj-dir} ${out-dir} ${obj-y}
	@echo "  >> LD $(TARGET)"
	${Q} $(LD) $(obj-y) $(LDFLAGS) -o $(out-dir)/$(TARGET)

