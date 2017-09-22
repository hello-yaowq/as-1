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
#include "Os.h"
#include "pci_core.h"
#include "asdebug.h"
#ifdef USE_LWIP
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/stats.h"
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip_handler.h"
#include "ethernetif.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_ETH 1

#define IFNAME0 't'
#define IFNAME1 'p'

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
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static pci_dev *pdev = NULL;
static void* __iobase= NULL;
static char buf[1514];
/* ============================ [ LOCALS    ] ====================================================== */
void Eth_Isr(void)
{
	if(__iobase != NULL)
	{
		uint32 flag;
		flag = readl(__iobase+REG_NETSTATUS);
		if(flag&FLG_RX)
		{
			#ifdef USE_LWIP
			extern struct netif* sys_get_netif(void);
			/* move received packet into a new pbuf */
			struct pbuf *p = low_level_input();

			if(p!=NULL){
				tcpip_input(p, sys_get_netif());
			}
			#endif
		}
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void PciNet_Init(uint32 gw, uint32 netmask, uint8* hwaddr, uint32* mtu)
{
	pdev = find_pci_dev_from_id(0xcaac,0x0002);
	if(NULL != pdev)
	{
		uint32 val;
		__iobase = (void*)(pdev->mem_addr[1]);

		enable_pci_resource(pdev);
		#ifdef __X86__
		pci_register_irq(pdev->irq_num,Eth_Isr);
		#else
		pci_bus_write_config_byte(pdev,0x3c,0x44);
		pci_register_irq(32+31,Eth_Isr);
		#endif
		enable_pci_interrupt(pdev);

		writel(__iobase+REG_GW, gw);
		writel(__iobase+REG_NETMASK, netmask);
		writel(__iobase+REG_CMD, 0);

		*mtu = readl(__iobase+REG_MTU);
		val = readl(__iobase+REG_MACL);
		hwaddr[0] = (val>>0)&0xFF;
		hwaddr[1] = (val>>8)&0xFF;
		hwaddr[2] = (val>>16)&0xFF;
		hwaddr[3] = (val>>24)&0xFF;
		val = readl(__iobase+REG_MACH);
		hwaddr[4] = (val>>0)&0xFF;
		hwaddr[5] = (val>>8)&0xFF;
	}
	else
	{
		ASLOG(ERROR,"No pci-asnet device found, specify '-device pci-asnet' to qemu\n");
	}
}
#ifdef USE_LWIP
struct pbuf * low_level_input(void)
{
	imask_t irq_state;
	struct pbuf *p, *q;
	u16_t len,len2,pos;
	char *bufptr;

	if(NULL == __iobase) return NULL;

	Irq_Save(irq_state);
	/* Obtain the size of the packet and put it into the "len"
	variable. */
	len = len2 = readl(__iobase+REG_LENGTH);

	pos = 0;
	while(len2 > 0)
	{
		buf[pos] = readl(__iobase+REG_DATA);
		pos ++;
		len2 --;
	}

	Irq_Restore(irq_state);
	if(0 == len) return NULL;

	/* We allocate a pbuf chain of pbufs from the pool. */
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

	if(p != NULL) {
		/* We iterate over the pbuf chain until we have read the entire
			packet into the pbuf. */
		bufptr = &buf[0];
		for(q = p; q != NULL; q = q->next) {
			/* Read enough bytes to fill this pbuf in the chain. The
			   available data in the pbuf is given by the q->len
			   variable. */
			/* read data into(q->payload, q->len); */
			memcpy(q->payload, bufptr, q->len);
			bufptr += q->len;
		}
	/* acknowledge that packet has been read(); */
	} else {
		/* drop packet(); */
	}

	return p;
}

err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q;
	char *bufptr;
	uint32 pos = 0;
	imask_t irq_state;

	(void)netif;

	if(NULL == __iobase) return ERR_ABRT;

	/* initiate transfer(); */

	bufptr = &buf[0];

	for(q = p; q != NULL; q = q->next) {
		/* Send the data from the pbuf to the interface, one pbuf at a
		time. The size of the data in each pbuf is kept in the ->len
		variable. */
		/* send data from(q->payload, q->len); */
		memcpy(bufptr, q->payload, q->len);
		bufptr += q->len;
	}

	Irq_Save(irq_state);
	/* signal that packet should be sent(); */
	writel(__iobase+REG_LENGTH,p->tot_len);
	while(p->tot_len > 0)
	{
		writel(__iobase+REG_DATA,*((uint8*)&buf[pos]));
		pos += 1;
		p->tot_len -= 1;
	}
	writel(__iobase+REG_CMD, 1);
	Irq_Restore(irq_state);
	return ERR_OK;
}

err_t tapif_init(struct netif *netif)
{
	uint32 mtu;
	PciNet_Init(netif->gw.addr, netif->netmask.addr, netif->hwaddr,&mtu);
	netif->hwaddr_len = 6;
	netif->mtu = mtu;

	ASLOG(ETH,"hwaddr is %02X:%02X:%02X:%02X:%02X:%02X, mtu=%d\n",
			netif->hwaddr[0],netif->hwaddr[1],netif->hwaddr[2],
			netif->hwaddr[3],netif->hwaddr[4],netif->hwaddr[5],
			mtu);
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;
	return ERR_OK;
}
#endif
#endif /* USE_PCI */
