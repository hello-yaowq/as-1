/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
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
#include "remoteproc.h"
#include <windows.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
static int start(struct rproc *rproc);
static int stop(struct rproc *rproc);
static void kick(struct rproc *rproc, int vqid);
/* ============================ [ DATAS     ] ====================================================== */
static struct device rpdev = {
	.name = "remote processor AUTOSAR ECU",
	.address = NULL,
	.size    = 0
};
static struct rproc* rproc;
static const struct rproc_ops rproc_ops=
{
	.start = start,
	.stop  = stop,
	.kick  = kick
};
/* ============================ [ LOCALS    ] ====================================================== */
static int start(struct rproc *rproc)
{
	return 0;
}
static int stop(struct rproc *rproc)
{
	return 0;
}
static void kick(struct rproc *rproc, int vqid)
{

}
/* ============================ [ FUNCTIONS ] ====================================================== */
void InitOS(void)
{
	if(NULL != rpdev.address)
	{
		printf(" >> start rproc up!\n");
		rproc = rproc_alloc(&rpdev,"AsRproc",&rproc_ops,NULL,1024);
	}
	else
	{
		assert(0);
	}
}

bool AsRproc_Init(void* address, size_t size,HANDLE r_lock,HANDLE w_lock,HANDLE r_event, HANDLE w_event, size_t sz_fifo)
{

	bool bOK = false;
	if(NULL == rpdev.address)
	{
		rpdev.address = address;
		rpdev.size    = size;
		rpdev.sz_fifo = sz_fifo;
		rpdev.r_lock  = r_lock;
		rpdev.w_lock  = w_lock;
		rpdev.r_event = r_event;
		rpdev.w_event = w_event;
		bOK = true;
	}
	printf("  >> AsRproc_Init(0x%X,%d,0x%X,0x%X,0x%X,0x%X) = %s\n",
			address,size,r_lock,w_lock,r_event,w_event,bOK?"true":"false");
	return bOK;
}
