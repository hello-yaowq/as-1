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
#include "lwip_handler.h"
#include "ethernetif.h"

#include "Os.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
err_t ethernetif_input(struct netif *netif, struct pbuf *p);
err_t ethernetif_init(struct netif *netif);
/* ============================ [ DATAS     ] ====================================================== */
static struct netif netif;
static boolean tcpip_initialized = FALSE;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	return 0;
}
void sys_sem_free(sys_sem_t *sem)
{

}

void sys_sem_set_invalid(sys_sem_t *sem)
{

}
int sys_sem_valid(sys_sem_t *sem)
{
	return TRUE;
}

void sys_sem_signal(sys_sem_t *sem)
{

}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	return 0;
}


err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	return 0;
}
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{

}
int sys_mbox_valid(sys_mbox_t *mbox)
{
	return TRUE;
}

void sys_mbox_free(sys_mbox_t *mbox)
{

}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	return 0;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return 0;
}static struct netif netif;

err_t ethernetif_input(struct netif *netif, struct pbuf *p);


err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	return 0;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{

}

void sys_init(void)
{

}

void pre_sys_init(void)
{

}

void netbios_init(void)
{

}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread,
		void *arg, int stacksize, int prio)
{
	return 0;
}

static void
tcpip_init_done(void *arg)
{
	tcpip_initialized = TRUE;
}
struct netif * LwIP_Init(void)
{
	uint8_t macaddress[6] = ETH_MAC_ADDR;
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

	/* Configure ethernet */
	Ethernet_Configuration();

#if NO_SYS
#if (MEM_LIBC_MALLOC==0)
	mem_init();
#endif
#if (MEMP_MEM_MALLOC==0)
	memp_init();
#endif
#else
	pre_sys_init();
	tcpip_init(tcpip_init_done, NULL);
	uint32 lockcnt = 0;
	while(tcpip_initialized == FALSE){
		lockcnt++;
		//SLEEP(0);
	};
#endif

#if LWIP_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	GET_BOOT_IPADDR;
	GET_BOOT_NETMASK;
	GET_BOOT_GW;
#endif

	Set_MAC_Address(macaddress);

	/* Add network interface to the netif_list */
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface.*/
	netif_set_default(&netif);

	#if LWIP_DHCP
	/* start dhcp search */
	dhcp_start(&netif);
	#else
	netif_set_addr(&netif, &ipaddr , &netmask, &gw);
	#endif

	/* netif is configured */
	netif_set_up(&netif);

	EnableEthDMAIrq();

	netbios_init();

	return &netif;
}

