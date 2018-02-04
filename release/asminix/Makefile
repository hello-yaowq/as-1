# Makefile for mini3 study
# http://wiki.minix3.org/doku.php?id=developersguide:crosscompiling

ARCH ?= i386

default:all

$(CURDIR)/minix:
	@(git clone https://github.com/Stichting-MINIX-Research-Foundation/minix.git)
	@(sudo cp -v $(CURDIR)/minix/etc/mk.conf /etc)

all:$(CURDIR)/minix
	@(cd minix && export DESTDIR=$(CURDIR)/out-$(ARCH) && ./build.sh -m$(ARCH) -E -O $(CURDIR)/build-$(ARCH) tools && \
		./build.sh -m$(ARCH) -O $(CURDIR)/build-${ARCH} -U distribution)

