/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "qemu/osdep.h"
#include "hw/hw.h"
#include "hw/pci/pci.h"
#include "qemu/event_notifier.h"
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
/* ============================ [ MACROS    ] ====================================================== */
#define TYPE_PCI_ASNET_DEV "pci-asnet"
#define PCI_ASNET_DEV(obj)     OBJECT_CHECK(PCIASNETDevState, (obj), TYPE_PCI_ASNET_DEV)
/* sizes must be power of 2 in PCI */
#define ASNET_IO_SIZE 1<<4
#define ASNET_MMIO_SIZE 1<<6

#define IFCONFIG_BIN "ifconfig "

#define DEVTAP "/dev/net/tun"
#define IFCONFIG_ARGS "%s inet %d.%d.%d.%d netmask %d.%d.%d.%d"

/* Get one byte from the 4-byte address */
#define ip4_addr1(ipaddr) (((uint8_t*)(ipaddr))[0])
#define ip4_addr2(ipaddr) (((uint8_t*)(ipaddr))[1])
#define ip4_addr3(ipaddr) (((uint8_t*)(ipaddr))[2])
#define ip4_addr4(ipaddr) (((uint8_t*)(ipaddr))[3])

enum {
	FLG_RX = 0x01,
	FLG_TX = 0x02,
};

enum{
	REG_MACL      = 0x00,
	REG_MACH      = 0x04,
	REG_MTU       = 0x08,
	REG_DATA      = 0x0C,
	REG_LENGTH    = 0x10,
	REG_NETSTATUS = 0x14,
	REG_GW        = 0x18,
	REG_NETMASK   = 0x1C,
	REG_CMD       = 0x20,
};
/* ============================ [ TYPES     ] ====================================================== */
typedef struct PCIASNETDevState {
	PCIDevice parent_obj;

	/* for PIO */
	MemoryRegion io;
	/* for MMIO */
	MemoryRegion mmio;
	/* irq used */
	qemu_irq irq;
	/* dma buf size */
	unsigned int dma_size;
	/* buffer copied with the dma operation on RAM */
	char *dma_buf;

	int fd;

	/** maximum transfer unit (in bytes) */
	uint32_t mtu;
	uint8_t hwaddr_len;
	uint8_t hwaddr[6];
	uint32_t gw;
	uint32_t netmask;

	uint32_t rxlength;
	uint32_t rxpos;
	uint32_t rxdata[2048/4];	/* hope the same CPU endian */

	uint32_t txlength;
	uint32_t txpos;
	uint32_t txdata[2048/4];	/* hope the same CPU endian */

	uint32_t flag;
} PCIASNETDevState;

static Property asnet_properties[] = {

DEFINE_PROP_END_OF_LIST(), };
/* ============================ [ DECLARES  ] ====================================================== */
static uint64_t asnet_mmioread(void *opaque, hwaddr addr, unsigned size);
static void asnet_mmiowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size);
static uint64_t asnet_ioread(void *opaque, hwaddr addr, unsigned size);
static void asnet_iowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size);
static void pci_asnetdev_class_init(ObjectClass *klass, void *data);
/* ============================ [ DATAS     ] ====================================================== */
/*
 * Callbacks called when the Memory Region
 * representing the MMIO space is
 * accessed.
 */
static const MemoryRegionOps asnet_mmio_ops = {
	.read = asnet_mmioread,
	.write = asnet_mmiowrite,
	.endianness = DEVICE_NATIVE_ENDIAN,
	.valid = {
		.min_access_size = 4,
		.max_access_size = 4,
	},
};

/*
 * Callbacks called when the Memory Region
 * representing the PIO space is
 * accessed.
 */
static const MemoryRegionOps asnet_io_ops = {
	.read = asnet_ioread,
	.write = asnet_iowrite,
	.endianness = DEVICE_NATIVE_ENDIAN,
	.valid = {
		.min_access_size = 4,
		.max_access_size = 4,
	},
};

/* Contains all the informations of the device
 * we are creating.
 * class_init will be called when we are defining
 * our device.
 */
static const TypeInfo pci_asnet_info = {
	.name = TYPE_PCI_ASNET_DEV,
	.parent = TYPE_PCI_DEVICE,
	.instance_size = sizeof(PCIASNETDevState),
	.class_init = pci_asnetdev_class_init,
};
/* ============================ [ LOCALS    ] ====================================================== */
static int low_level_probe(PCIASNETDevState *d,const char *name)
{
	int len;
	int s;
	struct ifreq ifr;

	len = strlen(name);
	if (len > (IFNAMSIZ-1)) {
		perror("tapif_init: name is too long");
		return -1;
	}
	s = socket(AF_INET,SOCK_DGRAM,0);
	if (s == -1) {
		perror("tapif_init: socket");
		return -1;
	}
	memset(&ifr,0,sizeof(ifr));
	strncpy(ifr.ifr_name,name,len);
	if (ioctl(s,SIOCGIFHWADDR,&ifr) == -1) {
		perror("tapif_init: ioctl SIOCGIFHWADDR");
		goto err;
	}
	uint8_t* hwaddr = (uint8_t*)&ifr.ifr_hwaddr.sa_data;
	d->hwaddr[0] = hwaddr[0];
	d->hwaddr[1] = hwaddr[1];
	d->hwaddr[2] = hwaddr[2];
	d->hwaddr[3] = hwaddr[3];
	d->hwaddr[4] = hwaddr[4];
	d->hwaddr[5] = hwaddr[5] ^ 1;
	d->hwaddr_len = 6;
	if (ioctl(s,SIOCGIFMTU,&ifr) == -1) {
		perror("tapif_init: ioctl SIOCGIFMTU");
		goto err;
	}
	d->mtu = ifr.ifr_mtu;
	close(s);
	return 0;
err:
	close(s);
	return -1;
}

static int low_level_output(PCIASNETDevState *d)
{
	/* signal that packet should be sent(); */
	if(write(d->fd, d->txdata, d->txlength) == -1) {
		perror("tapif: write");
		return -1;
	}
	return 0;
}

static int low_level_input(PCIASNETDevState *d)
{
	d->rxlength = read(d->fd, d->rxdata, sizeof(d->rxdata));

	if(d->rxlength > 0)
	{
		return 0;
	}

	return -1;
}

static void checkBus(PCIASNETDevState *d)
{
	if(0 == low_level_input(d))
	{
		d->flag |= FLG_RX;
	}
}

static void tapif_init(PCIASNETDevState *d,const char* name)
{
	char buf[sizeof(IFCONFIG_ARGS) + sizeof(IFCONFIG_BIN) + 50];
	struct ifreq ifr;
	d->fd = open(DEVTAP, O_RDWR);
	if(d->fd == -1) {
		perror("tapif_init: try running \"modprobe tun\" or rebuilding your kernel with CONFIG_TUN; cannot open "DEVTAP);
		return;
	}

	memset(&ifr, 0, sizeof(ifr));

	if (name != NULL)
	{
		strncpy(ifr.ifr_name,name,strlen(name));
	}

	ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
	if(ioctl(d->fd, TUNSETIFF, (void *) &ifr) < 0) {
		perror("tapif_init: "DEVTAP" ioctl TUNSETIFF");
		return;
	}

	if(!low_level_probe(d,name != NULL ? name : ifr.ifr_name))
	{
		perror("asnet low_level_probe failed");
		return;
	}

	if (name == NULL) {
		sprintf(buf, IFCONFIG_BIN IFCONFIG_ARGS,
			ifr.ifr_name,
			ip4_addr1(&(d->gw)),
			ip4_addr2(&(d->gw)),
			ip4_addr3(&(d->gw)),
			ip4_addr4(&(d->gw)),
			ip4_addr1(&(d->netmask)),
			ip4_addr2(&(d->netmask)),
			ip4_addr3(&(d->netmask)),
			ip4_addr4(&(d->netmask)));

		printf("tapif_init: system(\"%s\");\n", buf);
		if(system(buf))
		{
			perror("asnet system\n");
		}
	}
}
static void asnet_iowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size) {

	switch (addr) {
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
	}

}

static uint64_t asnet_ioread(void *opaque, hwaddr addr, unsigned size) {

	switch (addr) {
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
		return 0x0;
	}
}

static uint64_t asnet_mmioread(void *opaque, hwaddr addr, unsigned size) {
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;

	switch (addr) {
	case REG_MACL:
		return (d->hwaddr[0]+(d->hwaddr[1]<<8)+(d->hwaddr[2]<<16)+(d->hwaddr[3]<<24));
		break;
	case REG_MACH:
		return (d->hwaddr[4]+(d->hwaddr[5]<<8));
	case REG_MTU:
		return d->mtu;
		break;
	case REG_DATA:
	{
		uint32_t val;
		assert(d->rxpos < (sizeof(d->rxdata)/sizeof(d->rxdata[0])));
		val = d->rxdata[d->rxpos];
		d->rxpos++;

		return val;
		break;
	}
	case REG_LENGTH:
		d->rxpos = 0;
		return d->rxlength;
		break;
	case REG_NETSTATUS:
	{
		uint32_t flag;
		checkBus(d);
		flag = d->flag;
		d->flag = 0;
		return flag;
		break;
	}
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
		return 0x0;

	}
}

static void asnet_mmiowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size) {
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;

	switch (addr) {
	case REG_DATA:
		assert(d->txpos < (sizeof(d->txdata)/sizeof(d->txdata[0])));
		d->txdata[d->txpos] = value;
		d->txpos ++;
		break;
	case REG_LENGTH:
		assert(value <= sizeof(d->txdata));
		d->txlength = value;
		d->txpos = 0;
		break;
	case REG_GW:
		d->gw = value;
		break;
	case REG_NETMASK:
		d->netmask = value;
		break;
	case REG_CMD:
		switch (value)
		{
		case 0: /* init */
			if(d->fd)
			{
				printf ("%s: asnet alreay online!\n", __func__);
			}
			else
			{
				tapif_init(d,NULL);
			}
			break;
		case 1: /* TX */
			if(0 == low_level_output(d))
			{
				pci_irq_assert(d->parent_obj);
			}
			break;
		default:
			printf ("%s: unsupported command 0x%x\n", __func__, (int)value);
		}

		break;
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);

	}
}

/* Callbacks for MMIO and PIO regions are registered here */
static void asnet_io_setup(PCIASNETDevState *d) {
	memory_region_init_io(&d->mmio, OBJECT(d), &asnet_mmio_ops, d, "asnet_mmio",
			ASNET_MMIO_SIZE);
	memory_region_init_io(&d->io, OBJECT(d), &asnet_io_ops, d, "asnet_io",
			ASNET_IO_SIZE);
}

/* When device is loaded */
static int pci_asnetdev_init(PCIDevice *pci_dev) {
	/* init the internal state of the device */
	PCIASNETDevState *d = PCI_ASNET_DEV(pci_dev);
	d->dma_size = 0x1ffff * sizeof(char);
	d->dma_buf = malloc(d->dma_size);
	d->fd = -1;
	uint8_t *pci_conf;

	/* create the memory region representing the MMIO and PIO
	 * of the device
	 */
	asnet_io_setup(d);
	/*
	 * See linux device driver (Edition 3) for the definition of a bar
	 * in the PCI bus.
	 */
	pci_register_bar(pci_dev, 0, PCI_BASE_ADDRESS_SPACE_IO, &d->io);
	pci_register_bar(pci_dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY, &d->mmio);

	pci_conf = pci_dev->config;
	/* also in ldd, a pci device has 4 pin for interrupt
	 * here we use pin C.
	 */
	pci_conf[PCI_INTERRUPT_PIN] = 0x03;

	return 0;
}

/* When device is unloaded
 * Can be useful for hot(un)plugging
 */
static void pci_asnetdev_uninit(PCIDevice *dev) {
	PCIASNETDevState *d = (PCIASNETDevState *) dev;
	free(d->dma_buf);
}

static void qdev_pci_asnetdev_reset(DeviceState *dev) {
	PCIASNETDevState *d = (PCIASNETDevState *) dev;

	if(d->fd > 0)
	{
		close(d->fd);
	}

	d->flag = 0;
}

/* Called when the device is defined
 * PCI configuration is defined here
 * We inherit from PCIDeviceClass
 * Also see ldd for the meaning of the different args
 */
static void pci_asnetdev_class_init(ObjectClass *klass, void *data) {
	DeviceClass *dc = DEVICE_CLASS(klass);
	PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
	k->init = pci_asnetdev_init;
	k->exit = pci_asnetdev_uninit;
	/* this identify our device */
	k->vendor_id = 0xcaac;
	k->device_id = 0x0002;
	k->class_id = PCI_CLASS_OTHERS;
	set_bit(DEVICE_CATEGORY_MISC, dc->categories);

	k->revision = 0x00;
	dc->desc = "PCI ASNET";
	/* qemu user things */
	dc->props = asnet_properties;
	dc->reset = qdev_pci_asnetdev_reset;
}

/* function called before the qemu main
 * it will define our device
 */
static void pci_asnet_register_types(void) {
	type_register_static(&pci_asnet_info);
}
/* ============================ [ FUNCTIONS ] ====================================================== */
/* macro actually defining our device and registering it in qemu*/
type_init(pci_asnet_register_types);
