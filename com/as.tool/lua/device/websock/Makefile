target-y = aws
src-dir = $(CURDIR)
obj-dir = $(CURDIR)/obj
exe-dir = $(CURDIR)/out

prj-dir = $(subst /com/as.tool/lua/device/websock,,$(CURDIR))
INFRASTRUCTURE = $(prj-dir)/com/as.infrastructure
LUA   = $(prj-dir)/com/as.tool/lua
host = $(shell uname)

ifeq ($(host), Linux)
COMPILER_DIR ?= /usr
COMPILER_PREFIX =
cflags-y += -D__LINUX__
else
COMPILER_DIR ?= C:/MinGw
COMPILER_PREFIX =
cflags-y += -D__WINDOWS__
endif

cflags-y += -I$(INFRASTRUCTURE)/include
ldflags-y += -lwsock32

default:all

include $(prj-dir)/release/make/gcc.mk

all:lib

clean:clean-obj

