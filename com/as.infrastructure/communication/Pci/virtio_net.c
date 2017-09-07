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
#ifdef USE_PCI
/* ============================ [ INCLUDES  ] ====================================================== */
#include "virtio_net.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static pci_dev *pdev = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void virtio_net_init(void)
{
	int i = 0;
	pdev = find_pci_dev_from_id(0x1af4, 0x1000);
	enable_pci_resource(pdev);
	asmem("BAR4:",pdev->mem_addr[4],pdev->mem_size[4]);
	asmem("BAR5:",pdev->mem_addr[5],pdev->mem_size[5]);

	for(i=0 ; i < 256/4; i++)
	{
		if(i%4==0) printf("\n%02x::",4*i);
		printf("0x%08x,",pci_bus_read_config_dword(pdev,i*4));
	}
}

#endif /* USE_PCI */
