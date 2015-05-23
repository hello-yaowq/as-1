# specific target as libcan.a
target-y = can

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src
sip-dir = ./sip

LNFS  = python ../../make/lnfs.py

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=

FROM = ../../../com/as.tool/cancasexl.access
INFRASTRUCTURE = ../../../com/as.infrastructure/

vxlapi-dll = "C:\Users\Public\Documents\Vector XL Driver Library\bin\vxlapi.dll"
$(sip-dir):
	@mkdir -p $(sip-dir)
$(src-dir):
	@mkdir -p $(src-dir)	
	
dep:$(src-dir) $(sip-dir)
	@(cd $(src-dir);$(LNFS) $(FROM)/vxlapi.h vxlapi.h)
	@(cd $(src-dir);$(LNFS) $(FROM)/xlLoadlib.cpp xlLoadlib.c)
	@(cd $(src-dir);$(LNFS) $(INFRASTRUCTURE)/include/Std_Types.h Std_Types.h)
	@(cd $(src-dir);$(LNFS) $(FROM)/Can.h Can.h)
	@(cd $(src-dir);$(LNFS) $(FROM)/Can.c Can.c)
	@(cd $(sip-dir);$(LNFS) $(FROM)/Can.h Can.h)
	@(cd $(sip-dir);$(LNFS) $(INFRASTRUCTURE)/include/Std_Types.h Std_Types.h)
	@(cd $(sip-dir);$(LNFS) $(FROM)/can.sip can.sip)
	@(cd $(sip-dir);$(LNFS) $(FROM)/configure.py configure.py)
	@(cd $(sip-dir);$(LNFS) $(vxlapi-dll) vxlapi.dll)

include ../make/mingw.mk

pycan:
	@(cp -fv $(exe-dir)/lib$(target-y).a $(sip-dir)/lib$(target-y).a)
	@(cd $(sip-dir); make all)

sip2:
	@(cd $(sip-dir);python configure.py)
	@(cd $(sip-dir);sed -i "12 i\#include \"Can.h\"" sipAPICan.h)
	
all:lib sip2 pycan

clean:clean-obj-src
	@rm -frv $(sip-dir)/*
	
 

