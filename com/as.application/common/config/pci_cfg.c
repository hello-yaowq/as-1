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
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
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
