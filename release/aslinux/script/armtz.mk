# make file to study the arm trust-zone techology
# start from a clean directory, 
# ln -fs /as/release/aslinux/script/armtz.mk makefile

# arm or aarch64
ARCH ?= arm
qemu-tz:
	@git clone https://git.linaro.org/virtualization/qemu-tz.git
	@(cd qemu-tz;git submodule update --init dtc)

qemu-tztest:
	@git clone https://git.linaro.org/virtualization/qemu-tztest.git

qemu:
	@git clone git://git.qemu-project.org/qemu.git

asqemutz:qemu-tz
	@(cd qemu-tz;./configure;make)

asqemutztest:qemu-tztest
	@(cd qemu-tztest;./configure --arch=$(ARCH);make)

test:
ifeq ($(ARCH),arm)
	@(./qemu-tz/arm-softmmu/qemu-system-arm -bios ./qemu-tztest/tztest.img	\
        -serial stdio -display none -m 1024 -machine type=virt -cpu cortex-a15)
else
	@(./qemu-tz/aarch64-softmmu/qemu-system-aarch64 -bios ./qemu-tztest/tztest.img	\
		-serial stdio -display none -m 1024 -machine type=virt -cpu cortex-a57)
endif

all: asqemutz asqemutztest
	@(echo "  >> build done")

