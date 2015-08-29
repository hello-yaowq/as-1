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
const struct rproc_ops rproc_ops=
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
/**
 * rproc_alloc() - allocate a remote processor handle
 * @dev: the underlying device
 * @name: name of this remote processor
 * @ops: platform-specific handlers (mainly start/stop)
 * @firmware: name of firmware file to load, can be NULL
 * @len: length of private data needed by the rproc driver (in bytes)
 *
 * Allocates a new remote processor handle, but does not register
 * it yet. if @firmware is NULL, a default name is used.
 *
 * This function should be used by rproc implementations during initialization
 * of the remote processor.
 *
 * After creating an rproc handle using this function, and when ready,
 * implementations should then call rproc_add() to complete
 * the registration of the remote processor.
 *
 * On success the new rproc is returned, and on failure, NULL.
 *
 * Note: _never_ directly deallocate @rproc, even if it was not registered
 * yet. Instead, when you need to unroll rproc_alloc(), use rproc_put().
 */
struct rproc *rproc_alloc(struct device* dev, const char *name,
				const struct rproc_ops *ops,
				const char *firmware, int len)
{
	struct rproc *rproc;

	if (!dev || !name || !ops)
		return NULL;

	rproc = kzalloc(sizeof(struct rproc) + len, GFP_KERNEL);
	if (!rproc) {
		return NULL;
	}

	rproc->name = name;
	rproc->ops = ops;
	rproc->priv = &rproc[1];

	rproc->state = RPROC_OFFLINE;

	return rproc;
}
void InitOS(void)
{
	printf(" >> start rproc up!\n");
	rproc = rproc_alloc(&rpdev,"rproc",&rproc_ops,NULL,1024);
}

bool rproc_init(void* address, size_t size,HANDLE r_lock,HANDLE w_lock,HANDLE r_event, HANDLE w_event)
{

	bool bOK = false;
	if(NULL == rpdev.address)
	{
		rpdev.address = address;
		rpdev.size    = size;
		rpdev.r_lock  = r_lock;
		rpdev.w_lock  = w_lock;
		rpdev.r_event = r_event;
		rpdev.w_event = w_event;
		bOK = true;
	}
	printf("  >> rproc_init(0x%X,%d,0x%X,0x%X,0x%X,0x%X) = %s\n",
			address,size,r_lock,w_lock,r_event,w_event,bOK?"true":"false");
	return bOK;
}
