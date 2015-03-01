# specific
target-y = pyascom

obj-dir = ./obj
exe-dir = ./exe
src-dir   = ./src

def-y +=  -DDYNAMIC_XLDRIVER_DLL -DPOINTER_32=

$(src-dir):
	@mkdir -p $(src-dir)
	
dep:$(src-dir)
	@(cd $(src-dir);ln -fs ../../../com/clang/include/vxlapi.h vxlapi.h)
	@(cd $(src-dir);ln -fs ../../../com/clang/cplusplus/xlLoadlib.cpp xlLoadlib.cpp)

include ../make/Makefile

 

