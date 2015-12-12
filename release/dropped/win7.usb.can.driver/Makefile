# makefile for driver USB2CAN 
WDK = D:/WinDDK/7600.16385.1

CMD = C:/Windows/System32/cmd.exe

CC = $(WDK)/bin/x86/build.exe

ifeq ($(CC), $(wildcard $(CC)))
else
$(error fix your WDK compiler path $(CC) $(wildcard $(CC)))
endif
# Environment:
ia64_Checked_Build_Environment = $(WDK)/bin/setenv.bat $(WDK) chk ia64 WIN7 no_oacr
ia64_Free_Build_Environment    = $(WDK)/bin/setenv.bat $(WDK) fre ia64 WIN7 no_oacr 
x64_Checked_Build_Environment  = $(WDK)/bin/setenv.bat $(WDK) chk x64 WIN7
x64_Free_Build_Environment     = $(WDK)/bin/setenv.bat $(WDK) fre x64 WIN7
x86_Checked_Build_Environment  = $(WDK)/bin/setenv.bat $(WDK) chk x86 WIN7
x86_Free_Build_Environment     = $(WDK)/bin/setenv.bat $(WDK) fre x86 WIN7

# choose BUILD_ENV
BUILD_ENV = $(x86_Free_Build_Environment)

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
#	@(cd $(src-dir);$(LNFS) $(prj-dir)/com/driver/windows.usb.can TRUE)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/api)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/atl71)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/crt)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/ddk)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/mfc42)
	@(cd $(inc-dir);$(LNFS) $(WDK)/inc/wdf)
	
	@(cd $(src-dir);$(LNFS) $(WDK)/src/usb/usbsamp/sys)

all:$(obj-dir)
	
clean: 
	@rm -fv $(obj-dir)/*
	@rm -fv $(exe-dir)/*
	
clean-dist:clean
	@rm -fv $(src-dir)/*	