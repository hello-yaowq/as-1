# Makefile for asvirt-remoteproc

# native, arm, x86
ARCH ?= native

ifeq ($(ARCH),native)
BUILDER ?= /lib/modules/$(shell uname -r)/build
endif

obj-m += asvirt_remoteproc.o
obj-m += rproc-asvirt/RPmsg.o \
		rproc-asvirt/RPmsg_Cfg.o \
		rproc-asvirt/Rproc_Cfg.o \
		rproc-asvirt/VirtQ.o \
		rproc-asvirt/VirtQ_Cfg.o \
		rproc-asvirt/Ipc_Cfg.o \
		rproc-asvirt/Ipc.o

default:all

dep:
	@(cd rproc-asvirt;make dep)

all:
	$(MAKE) -C $(BUILDER) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILDER) M=$(PWD) clean
