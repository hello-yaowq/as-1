
obj-$(CONFIG_PCI) += ascan.o
obj-$(CONFIG_PCI) += asnet.o
obj-$(CONFIG_PCI) += asblk.o
LIBS += -lpyas -L../hw/char -L./hw/char

test:
	gcc -D__TAPTEST__ asnet.c -o asnet
