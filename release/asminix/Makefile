# Makefile for mini3 study
# http://wiki.minix3.org/doku.php?id=developersguide:crosscompiling

ARCH ?= i386

default:all

$(CURDIR)/minix:
	@(git clone https://github.com/Stichting-MINIX-Research-Foundation/minix.git)

all:$(CURDIR)/minix
	@(cd minix && ./build.sh -m$(ARCH) -O $(CURDIR)/build-$(ARCH) tools && \
		./build.sh -m$(ARCH) -O $(CURDIR)/build-${ARCH} -U distribution)

