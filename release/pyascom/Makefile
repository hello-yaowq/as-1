# specific target as libvxlapi.a
target-y = vxlapi

obj-dir = ./obj
exe-dir = ./src
src-dir = ./src

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=

dep:$(src-dir)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/vxlapi.h vxlapi.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/cplusplus/xlLoadlib.cpp xlLoadlib.c)
	@(cd $(src-dir);ln -fs ../../../com/python/sip/vxlapi.sip vxlapi.sip)
	@(cd $(src-dir);ln -fs ../../../com/python/sip/configure.py configure.py)

include ../make/Makefile

pyascom:
	@(cd $(src-dir); make all)

sip:
	@(cd $(src-dir);python configure.py)
	@(cd $(src-dir);sed -i "12 i\#include <windows.h>" sipAPIvxlapi.h)
	@(cd $(src-dir);sed -i "13 i\#include \"vxlapi.h\"" sipAPIvxlapi.h)

#pre-process vxlapi.h	
vxlapi:
	@(cd $(src-dir);gcc -E vxlapi.h -D__int64=long -DHANDLE=void* -DPOINTER_32= -o vxlapi2.h)
	@(cd $(src-dir);sed -i "s/\#/\/\/\#/g" vxlapi2.h)
	@(cd $(src-dir);sed -i "s/__attribute__((dllimport)) __attribute__((__stdcall__))/ /g" vxlapi2.h)
	@(cd $(src-dir);sed -i "1 i\#ifndef VXLAPI2_H\n\#define VXLAPI2_H\n\n" vxlapi2.h) 
	@(cd $(src-dir);echo "#endif /* VXLAPI2_H */" >> vxlapi2.h)
	
	
all:clean dep lib vxlapi sip pyascom

	
 

