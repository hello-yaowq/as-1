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
 * 
 * This is copied from [tinyos](https://github.com/ddk50/tinyos) pci-main
 */
#ifndef _PCI_CORE_H_
#define _PCI_CORE_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "pci_ids.h"
#include "pci.h"

/* ============================ [ MACROS    ] ====================================================== */
#define     PCI_DEVICE(vid, did, name)                              \
	{PCI_VENDOR_ID_##vid, PCI_DEVICE_ID_##did, (name), 0xff}

#define     PCI_BRIDGE(vid, did, name, bridge)                          \
	{PCI_VENDOR_ID_##vid, PCI_DEVICE_ID_##did, (name), (bridge)}

/* ============================ [ TYPES     ] ====================================================== */
typedef struct __pci_vendor_info {
	WORD			vendor_id;			/* vendor id */
	WORD			device_id;			/* device id */
	const char		*name;			/* device name */
	BYTE			bridge_type;	/* bridge type or 0xff */
} pci_vendor_info;

typedef struct __pci_device_info {
	pci_vendor_info		info;
	DWORD				base_addr;
	BYTE				base_class;
	BYTE				sub_class;
} pci_device_info;

typedef struct __pci_res {
	BYTE		bus;		/* バス番号 */
	BYTE		dev;		/* デバイス番号 */
	BYTE		fn;			/* 機能番号 */
} pci_reg;

typedef struct __pci_dev {
	int						pci_num;
	struct __pci_dev		*next;
	DWORD					mem_addr[6];
	DWORD					mem_size[6];
	WORD					io_addr[6];
	WORD					io_size[6];
	BYTE					irq_num;
	BYTE					intr_num;
	WORD					vendor_id;
	WORD					device_id;
	WORD					subsys_id;
	pci_vendor_info			*vendor;
	pci_reg					dev;
} pci_dev;


#include "pci_cfg.h"
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int pci_generic_config_write(unsigned int busnr, unsigned int devfn,
							 int where, int size, u32 val);
int pci_generic_config_read(unsigned int busnr, unsigned int devfn,
							int where, int size, u32 *val);

BYTE pci_read_config_reg8(pci_reg*, BYTE);
WORD pci_read_config_reg16(pci_reg*, BYTE);
DWORD pci_read_config_reg32(pci_reg*, BYTE);

void pci_write_config_reg8(pci_reg*, BYTE, const BYTE);
void pci_write_config_reg16(pci_reg*, BYTE, const WORD);
void pci_write_config_reg32(pci_reg*, BYTE, const DWORD);

void disable_pci_resource(pci_dev*);
void enable_pci_resource(pci_dev*);

void enable_pci_interrupt(pci_dev*);
void disable_pci_interrupt(pci_dev*);

int pci_register_irq(DWORD irq_num, void (*handler)());
int pci_unregister_irq(DWORD irq_num);

pci_dev *find_pci_dev_from_reg(pci_reg *reg);
pci_dev *find_pci_dev_from_id(DWORD vendor_id, DWORD device_id);

void pci_search_all_device(void);
#endif /* _PCI_CORE_H_ */
