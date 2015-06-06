# makefile for driver USB2CAN 
WDK = D:/WinDDK/7600.16385.1



CC = $(WDK)/bin

ifeq ($(CC), $(wildcard $(CC)))
else
$(error fix your WDK compiler path $(CC) $(wildcard $(CC)))
endif

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src
inc-dir = ./include

inc-y += /I $(inc-dir)

prj-dir = $(subst release/win7.usb.can.driver,,$(CURDIR))

LNFS  = python $(prj-dir)/release/make/lnfs.py

$(inc-dir):
	@mkdir -p $@
	
$(src-dir):
	@mkdir -p $@
	
$(obj-dir):
	@mkdir -p $@
	
dep:$(src-dir) $(inc-dir)
	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/driver/windows.usb.can TRUE)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/api)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/atl71)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/crt)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/ddk)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/mfc42)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/wdf)

all:$(obj-dir)
	@(echo $(CC))
	
clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-dist:clean
	@rm -fv $(src-dir)/*	