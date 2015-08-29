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
struct firmware {
	size_t size;
	const u8 *data;
	struct page **pages;

	/* firmware loader private fields */
	void *priv;
};
/**
 * struct rproc_fw_ops - firmware format specific operations.
 * @find_rsc_table:	find the resource table inside the firmware image
 * @find_loaded_rsc_table: find the loaded resouce table
 * @load:		load firmeware to memory, where the remote processor
 *			expects to find it
 * @sanity_check:	sanity check the fw image
 * @get_boot_addr:	get boot address to entry point specified in firmware
 */
struct rproc_fw_ops {
	struct resource_table *(*find_rsc_table) (struct rproc *rproc,
						const struct firmware *fw,
						int *tablesz);
	struct resource_table *(*find_loaded_rsc_table)(struct rproc *rproc,
						const struct firmware *fw);
	int (*load)(struct rproc *rproc, const struct firmware *fw);
	int (*sanity_check)(struct rproc *rproc, const struct firmware *fw);
	u32 (*get_boot_addr)(struct rproc *rproc, const struct firmware *fw);
};
/* ============================ [ DECLARES  ] ====================================================== */
static void rproc_type_release(struct device *dev);
/* ============================ [ DATAS     ] ====================================================== */
static void*  l_rsc_tbl_address = NULL;
static size_t l_rsc_tbl_size   = 0;
static HANDLE l_r_lock = NULL;
static HANDLE l_w_lock = NULL;
static HANDLE l_r_event = NULL;
static HANDLE l_w_event = NULL;
/* ============================ [ LOCALS    ] ====================================================== */

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
struct rproc *rproc_alloc(void *dev, const char *name,
				const struct rproc_ops *ops,
				const char *firmware, int len)
{
	struct rproc *rproc;
	char *p, *template = "rproc-%s-fw";
	int name_len = 0;

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

bool rproc_init(void* address, size_t size,HANDLE r_lock,HANDLE w_lock,HANDLE r_event, HANDLE w_event)
{

	bool bOK = false;
	if(NULL == l_rsc_tbl_address)
	{
		l_rsc_tbl_address = address;
		l_rsc_tbl_size    = size;
		l_r_lock          = r_lock;
		l_w_lock          = w_lock;
		l_r_event         = r_event;
		l_w_event         = w_event;
		bOK = true;
	}
	printf("  >> rproc_init(0x%X,%d,0x%X,0x%X,0x%X,0x%X) = %s\n",
			address,size,r_lock,w_lock,r_event,w_event,bOK?"true":"false");
	return bOK;
}
