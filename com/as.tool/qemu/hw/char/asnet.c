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
/* ============================ [ MACROS    ] ====================================================== */
#define TYPE_PCI_ASNET_DEV "pci-asnet"
#define PCI_ASNET_DEV(obj)     OBJECT_CHECK(PCIASNETDevState, (obj), TYPE_PCI_ASNET_DEV)
/* sizes must be power of 2 in PCI */
#define ASNET_IO_SIZE 1<<4
#define ASNET_MMIO_SIZE 1<<6
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
	/* did we throw an interrupt ? */
	int threw_irq;
	/* id of the device, writable */
	int id;
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
static void asnet_iowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size) {
	int i;
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;
	PCIDevice *pci_dev = (PCIDevice *) opaque;

	switch (addr) {
	case 0:
		if (value) {
			/* throw an interrupt */
			d->threw_irq = 1;
			pci_irq_assert(pci_dev);

		} else {
			/*  ack interrupt */
			pci_irq_deassert(pci_dev);
			d->threw_irq = 0;
		}
		break;
	case 4:
		/* throw a random DMA */
		for (i = 0; i < d->dma_size; ++i)
			d->dma_buf[i] = rand();
		cpu_physical_memory_write(value, (void *) d->dma_buf, d->dma_size);
		break;
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
	}

}

static uint64_t asnet_ioread(void *opaque, hwaddr addr, unsigned size) {
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;

	switch (addr) {
	case 0:
		/* irq status */
		return d->threw_irq;
		break;
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
		return 0x0;
	}
}

static uint64_t asnet_mmioread(void *opaque, hwaddr addr, unsigned size) {
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;

	switch (addr) {
	case 0:
		/* also irq status */
		return d->threw_irq;
		break;
	case 4:
		/* Id of the device */
		return d->id;
		break;
	default:
		printf ("%s: Bad register offset 0x%x\n", __func__, (int)addr);
		return 0x0;

	}
}

static void asnet_mmiowrite(void *opaque, hwaddr addr, uint64_t value,
		unsigned size) {
	PCIASNETDevState *d = (PCIASNETDevState *) opaque;
	PCIDevice *pci_dev = (PCIDevice *) opaque;

	switch (addr) {
	case 4:
		/* change the id */
		d->id = value;
		break;
	case 8:
		if (value) {
			/* throw an interrupt */
			d->threw_irq = 1;
			pci_irq_assert(pci_dev);

		} else {
			/*  ack interrupt */
			pci_irq_deassert(pci_dev);
			d->threw_irq = 0;
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
	d->id = 0xcaac;
	d->threw_irq = 0;
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
	 * here we use pin B.
	 */
	pci_conf[PCI_INTERRUPT_PIN] = 0x02;

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
	/* TODO init here */
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
