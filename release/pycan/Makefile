# specific target as libcan.a
target-y = can

obj-dir = ./obj
exe-dir = ./out
src-dir = ./src
sip-dir = ./sip

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=

vxlapi-dll = "C:\Users\Public\Documents\Vector XL Driver Library\bin\vxlapi.dll"
$(sip-dir):
	@mkdir -p $(sip-dir)
$(src-dir):
	@mkdir -p $(src-dir)	
	
dep:$(src-dir) $(sip-dir)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/vxlapi.h vxlapi.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/cplusplus/xlLoadlib.cpp xlLoadlib.c)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/Std_Types.h Std_Types.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/Can.h Can.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/c/Can.c Can.c)
	@(cd $(sip-dir);ln -fs ../../../com/clang/include/Can.h Can.h)
	@(cd $(sip-dir);ln -fs ../../../com/clang/include/Std_Types.h Std_Types.h)
	@(cd $(sip-dir);ln -fs ../../../com/python/sip/can.sip can.sip)
	@(cd $(sip-dir);ln -fs ../../../com/python/sip/configure.py configure.py)
	@(cd $(sip-dir);ln -fs $(vxlapi-dll) vxlapi.dll)

include ../make/Makefile

pycan:
	@(cp -fv $(exe-dir)/lib$(target-y).a $(sip-dir)/lib$(target-y).a)
	@(cd $(sip-dir); make all)

sip2:
	@(cd $(sip-dir);python configure.py)
	@(cd $(sip-dir);sed -i "12 i\#include \"Can.h\"" sipAPICan.h)
	
all:clean2 dep lib sip2 pycan

clean2:clean
	@rm -frv $(sip-dir)/*
	
 

