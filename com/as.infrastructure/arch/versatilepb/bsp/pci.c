/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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
#ifdef USE_PCI
/* ============================ [ INCLUDES  ] ====================================================== */
#define IO_VALUE_FIRST
#include "Std_Types.h"
#include "asdebug.h"
#include "pci.h"

typedef uint32_t u32;
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_PCI 1

#define PCI_IMAP(m)		(versatile_pci_base + ((m) * 4))
#define PCI_SMAP(m)		(versatile_pci_base + 0x14 + ((m) * 4))
#define PCI_SELFID		(versatile_pci_base + 0xc)

#define VP_PCI_DEVICE_ID		0x030010ee
#define VP_PCI_CLASS_ID			0x0b400000

#define PHYS_OFFSET 0
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* according to https://github.com/qemu/qemu/blob/master/hw/arm/versatilepb.c */
static void *versatile_pci_base = 0x10001000;
static void *versatile_cfg_base[2] = { 0x41000000, 0x42000000 };
static u32 pci_slot_ignore;
/* ============================ [ LOCALS    ] ====================================================== */
static int versatile_pci_probe(void)
{
	int ret, i, myslot = -1;
	u32 val;
	void *local_pci_cfg_base;

	/*
	 * We need to discover the PCI core first to configure itself
	 * before the main PCI probing is performed
	 */
	for (i = 0; i < 32; i++) {
		if ((readl(versatile_cfg_base[0] + (i << 11) + PCI_VENDOR_ID) == VP_PCI_DEVICE_ID) &&
		    (readl(versatile_cfg_base[0] + (i << 11) + PCI_CLASS_REVISION) == VP_PCI_CLASS_ID)) {
			myslot = i;
			break;
		}
	}
	if (myslot == -1) {
		ASLOG(PCI, "Cannot find PCI core!\n");
		return -__LINE__;
	}
	/*
	 * Do not to map Versatile FPGA PCI device into memory space
	 */
	pci_slot_ignore |= (1 << myslot);

	ASLOG(PCI, "PCI core found (slot %d)\n", myslot);

	writel(myslot, PCI_SELFID);
	local_pci_cfg_base = versatile_cfg_base[1] + (myslot << 11);

	val = readl(local_pci_cfg_base + PCI_COMMAND);
	val |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE;
	writel(val, local_pci_cfg_base + PCI_COMMAND);

	/*
	 * Configure the PCI inbound memory windows to be 1:1 mapped to SDRAM
	 */
	writel(PHYS_OFFSET, local_pci_cfg_base + PCI_BASE_ADDRESS_0);
	writel(PHYS_OFFSET, local_pci_cfg_base + PCI_BASE_ADDRESS_1);
	writel(PHYS_OFFSET, local_pci_cfg_base + PCI_BASE_ADDRESS_2);

	/*
	 * For many years the kernel and QEMU were symbiotically buggy
	 * in that they both assumed the same broken IRQ mapping.
	 * QEMU therefore attempts to auto-detect old broken kernels
	 * so that they still work on newer QEMU as they did on old
	 * QEMU. Since we now use the correct (ie matching-hardware)
	 * IRQ mapping we write a definitely different value to a
	 * PCI_INTERRUPT_LINE register to tell QEMU that we expect
	 * real hardware behaviour and it need not be backwards
	 * compatible for us. This write is harmless on real hardware.
	 */
	writel(0, versatile_cfg_base[0] + PCI_INTERRUPT_LINE);

	return 0;
}

/* ============================ [ FUNCTIONS ] ====================================================== */
int pci_init(void)
{
	int rv;

	rv = versatile_pci_probe();

	return rv;
}
#endif
