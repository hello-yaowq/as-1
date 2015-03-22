# specific target as libvxlapi.a
target-y = vxlapi

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
	@(cd $(sip-dir);ln -fs ../../../com/clang/include/vxlapi.h vxlapi.h)
	@(cd $(sip-dir);ln -fs ../../../com/python/sip/vxlapi.sip vxlapi.sip)
	@(cd $(sip-dir);ln -fs ../../../com/python/sip/configure.py configure.py)
	@(cd $(sip-dir);ln -fs ../../../com/python/sip/test.py test.py)
	@(cd $(sip-dir);ln -fs ../../../com/python/can/vxlapih.py vxlapih.py)
	@(cd $(sip-dir);ln -fs $(vxlapi-dll) vxlapi.dll)

include ../make/Makefile

pyascom:
	@(cp -fv $(exe-dir)/lib$(target-y).a $(sip-dir)/lib$(target-y).a)
	@(cd $(sip-dir); make all)

sip2:
	@(cd $(sip-dir);python configure.py)
	@(cd $(sip-dir);sed -i "12 i\#include <windows.h>" sipAPIvxlapi.h)
	@(cd $(sip-dir);sed -i "13 i\#include \"vxlapi.h\"" sipAPIvxlapi.h)

#pre-process vxlapi.h	
vxlapi:
	@(cd $(sip-dir);gcc -E vxlapi.h -D__int64=long -DHANDLE=void* -DPOINTER_32= -o vxlapi2.h)
	@(cd $(sip-dir);sed -i "s/\#/\/\/\#/g" vxlapi2.h)
	@(cd $(sip-dir);sed -i "s/__attribute__((dllimport)) __attribute__((__stdcall__))/ /g" vxlapi2.h)
	@(cd $(sip-dir);sed -i "1 i\#ifndef VXLAPI2_H\n\#define VXLAPI2_H\n\n" vxlapi2.h) 
	@(cd $(sip-dir);echo "#endif /* VXLAPI2_H */" >> vxlapi2.h)
	
	
all:clean2 dep lib vxlapi sip2 pyascom

clean2:clean
	@rm -frv $(sip-dir)/*
	
 

