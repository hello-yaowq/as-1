# Make file that for the study purpose of lintom on host machine ubuntu

target-y = aslibtom

obj-dir = $(CURDIR)/obj
exe-dir = $(CURDIR)/out
src-dir = $(CURDIR)/src
inc-dir = $(CURDIR)/src

download = $(CURDIR)/../download

host = $(shell uname)

ifeq ($(host), Linux)
PY27 = python2
PY34 = python3
else
PY27 = C:Python27/python
PY34 = C:Python34/python
endif

LNFS  = $(PY34) $(CURDIR)/../make/lnfs.py

### flags for libtom
def-y += -DLTC_NO_ASM
cflags-y += -Wall -Wsign-compare -W -Wshadow -Wno-unused-parameter -DLTC_SOURCE  -funroll-loops -fomit-frame-pointer 
cflags-y += --include "tomcrypt_macros.h"

ldflags-y += -lpthread
ldflags-y += -lstdc++ -lm

include ../make/posix.mk

dep: $(src-dir)
	@(cd $(src-dir); $(LNFS) $(download)/libtomcrypt-1.17/src TRUE)

all: $(src-dir) $(download) exe


clean: clean-obj

$(src-dir):
	@mkdir -p $@

$(download):
	@mkdir -p $@

$(download)/crypt-1.17.tar.bz2:
	@(cd $(download);	\
		wget https://github.com/libtom/libtomcrypt/releases/download/1.17/crypt-1.17.tar.bz2;	\
		)

$(download)/libtomcrypt-1.17:$(download)/crypt-1.17.tar.bz2
	@(cd $(download);	\
		bzip2 -dk $(download)/crypt-1.17.tar.bz2;	\
		tar -xf $(download)/crypt-1.17.tar -C .;	\
		rm $(download)/crypt-1.17.tar )

$(download)/ltm-1.0.tar.xz:
	@(cd $(download);	\
		wget https://github.com/libtom/libtommath/releases/download/v1.0/ltm-1.0.tar.xz;	\
		)

$(download)/libtommath-1.0:$(download)/ltm-1.0.tar.xz
	@(cd $(download);	\
		xz -dk $(download)/ltm-1.0.tar.xz;	\
		tar -xf $(download)/ltm-1.0.tar -C .;	\
		rm $(download)/ltm-1.0.tar )

$(download)/tfm-0.13.tar.bz2:
	@(cd $(download);	\
		wget https://github.com/libtom/tomsfastmath/releases/download/v0.13.0/tfm-0.13.tar.bz2;	\
		)

$(download)/tomsfastmath-0.13:$(download)/tfm-0.13.tar.bz2
	@(cd $(download);	\
		bzip2 -dk $(download)/tfm-0.13.tar.bz2;	\
		tar -xf $(download)/tfm-0.13.tar -C .;	\
		rm $(download)/tfm-0.13.tar )

$(download)/ltp-0.04.tar.bz2:
	@(cd $(download);	\
		wget https://github.com/libtom/libtompoly/releases/download/0.04/ltp-0.04.tar.bz2;	\
		)

$(download)/libtompoly-0.04:$(download)/ltp-0.04.tar.bz2
	@(cd $(download);	\
		bzip2 -dk $(download)/ltp-0.04.tar.bz2;	\
		tar -xf $(download)/ltp-0.04.tar -C .;	\
		rm $(download)/ltp-0.04.tar )

$(download)/ltf-0.02.tar.bz2:
	@(cd $(download);	\
		wget https://github.com/libtom/libtomfloat/releases/download/0.02/ltf-0.02.tar.bz2;	\
		)

$(download)/libtomfloat-0.02:$(download)/ltf-0.02.tar.bz2
	@(cd $(download);	\
		bzip2 -dk $(download)/ltf-0.02.tar.bz2;	\
		tar -xf $(download)/ltf-0.02.tar -C .;	\
		rm $(download)/ltf-0.02.tar )

aslibtom:	$(download)/libtomcrypt-1.17	\
			$(download)/libtommath-1.0		\
			$(download)/tomsfastmath-0.13	\
			$(download)/libtompoly-0.04		\
			$(download)/libtomfloat-0.02
