target-y ?= ff13

src-dir = $(CURDIR)
obj-dir = $(CURDIR)/obj
exe-dir = $(CURDIR)/out

prj-dir = $(subst /com/as.infrastructure/system/fs,,$(CURDIR))
INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
download = $(prj-dir)/release/download

ifeq ($(target-y),ff13)
dir-y += $(download)/ff13/source
endif

default:all

include $(prj-dir)/release/make/gcc.mk

$(download)/ff13.zip:
	@(cd $(download);wget http://elm-chan.org/fsw/ff/arc/ff13.zip)

$(download)/ff13:$(download)/ff13.zip
	@(cd $(download);mkdir ff13;cd ff13;unzip $(download)/ff13.zip; rm source/diskio.c)

$(download)/ffsample.zip:
	@(cd $(download);wget http://elm-chan.org/fsw/ff/ffsample.zip)

$(download)/ffsample:$(download)/ffsample.zip
	@(cd $(download);mkdir ffsample;cd ffsample;unzip $(download)/ffsample.zip)
	
aslibff13:$(download)/ff13
#$(download)/ffsample

all: aslib$(target-y)
	make lib

clean:clean-obj