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
#include "pci_core.h"
/* ============================ [ MACROS    ] ====================================================== */
#define PCI_DEVICE_ID_QEMU PCI_SUBDEVICE_ID_QEMU
#define PCI_DEVICE_ID_0300 0x0300
#define PCI_DEVICE_ID_1000 0x1000
#define PCI_DEVICE_ID_1001 0x1001
#define PCI_DEVICE_ID_1005 0x1005
#define PCI_DEVICE_ID_003F 0x003F

#define PCI_VENDOR_ID_HELLO_TIC 0x1337
#define PCI_DEVICE_ID_0001 0x0001

#define mmiocfg_NCR_LSI_53C895A      NULL
#define mmiocfg_APPLE_003F           NULL
#define mmiocfg_XILINX_0300          NULL
#define mmiocfg_REDHAT_QUMRANET_1005 NULL
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static uint32 __attribute__((aligned(16))) __attribute__((section(".pcip"))) memp_REDHAT_QUMRANET_1000[0x20];
static uint32 __attribute__((aligned(16))) __attribute__((section(".pcinp"))) memnp_REDHAT_QUMRANET_1000[0x10];
static const pci_mmio_cfg mmiocfg_REDHAT_QUMRANET_1000[1] =
{
	{
		.mem_addr = {0,0,0,0,(DWORD)memp_REDHAT_QUMRANET_1000,(DWORD)memnp_REDHAT_QUMRANET_1000},
		.mem_size={0,0,0,0,0x20,0x10},
		.io_addr = {0,0,0,0,0,0},
		.io_size={0,0,0,0,0,0},
	}
};

static uint32 __attribute__((aligned(16))) __attribute__((section(".pcip"))) memp_REDHAT_QUMRANET_1001[0x40];
static uint32 __attribute__((aligned(16))) __attribute__((section(".pcinp"))) memnp_REDHAT_QUMRANET_1001[0x10];
static const pci_mmio_cfg mmiocfg_REDHAT_QUMRANET_1001[1] =
{
	{
		.mem_addr = {0,0,0,0,(DWORD)memp_REDHAT_QUMRANET_1001,(DWORD)memnp_REDHAT_QUMRANET_1001},
		.mem_size={0,0,0,0,0x40,0x10},
		.io_addr = {0,0,0,0,0,0},
		.io_size={0,0,0,0,0,0},
	}
};

static uint32 __attribute__((aligned(16))) __attribute__((section(".pcinp"))) mem_HELLO_TIC_0001[0x40];
static const pci_mmio_cfg mmiocfg_HELLO_TIC_0001[1] =
{
	{
		.mem_addr = {0,(DWORD)mem_HELLO_TIC_0001,0,0,0,0},
		.mem_size={0,0x40,0,0,0,0},
		.io_addr = {0,0,0,0,0,0},
		.io_size={0,0,0,0,0,0},
	}
};

/* in the order of vendor id and then device id, both from small to big */
/* https://github.com/qemu/qemu/blob/master/docs/specs/pci-ids.txt */
const pci_vendor_info  pci_vendor_list[PCI_VENDOR_NUM] = 
{
	PCI_DEVICE( NCR,          LSI_53C895A,    "NCR LSI_53C895A"),
	PCI_DEVICE( APPLE,               003F,    "apple ?"),
	PCI_DEVICE( XILINX,              0300,    "xilinx ?"),
	PCI_DEVICE( HELLO_TIC,           0001,    "hello tic"),
	PCI_DEVICE( REDHAT_QUMRANET,     1000,    "REDHAT_QUMRANET network device (legacy)"),
	PCI_DEVICE( REDHAT_QUMRANET,     1001,    "REDHAT_QUMRANET block device (legacy)"),
	PCI_DEVICE( REDHAT_QUMRANET,     1005,    "REDHAT_QUMRANET entropy generator device (legacy)"),
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* USE_PCI */
