# specific target as libvxlapi.a
target-y = vxlapi

obj-dir = ./obj
exe-dir = ./src
src-dir = ./src

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=

dep:$(src-dir)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/vxlapi.h vxlapi.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/cplusplus/xlLoadlib.cpp xlLoadlib.cpp)
	@(cd $(src-dir);ln -fs ../../../com/python/sip/vxlapi.sip vxlapi.sip)
	@(cd $(src-dir);ln -fs ../../../com/python/sip/configure.py configure.py)

include ../make/Makefile

pyascom:
	@(cd $(src-dir); make all)

sip:
	@(cd $(src-dir);python configure.py)

#pre-process vxlapi.h	
vxlapi:
	@(cd $(src-dir);gcc -E vxlapi.h -D__int64=long -DHANDLE=void* -DPOINTER_32= -o vxlapi2.h)
	@(cd $(src-dir);sed -i "s/\#/\/\/\#/g" vxlapi2.h)
	
	
all:lib vxlapi sip pyascom

	
 

