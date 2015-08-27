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
static int  rproc_elf_load_segments(struct rproc *rproc, const struct firmware *fw);
static struct resource_table *
rproc_elf_find_rsc_table(struct rproc *rproc, const struct firmware *fw,
			 int *tablesz);
static struct resource_table *
rproc_elf_find_loaded_rsc_table(struct rproc *rproc, const struct firmware *fw);
static int
rproc_elf_sanity_check(struct rproc *rproc, const struct firmware *fw);
static
u32 rproc_elf_get_boot_addr(struct rproc *rproc, const struct firmware *fw);
/* ============================ [ DATAS     ] ====================================================== */
static struct device_type rproc_type = {
	.name		= "remoteproc",
	.release	= rproc_type_release,
};
static void*  l_rsc_tbl_address = NULL;
static size_t l_rsc_tbl_size   = 0;
static HANDLE l_r_lock = NULL;
static HANDLE l_w_lock = NULL;
static HANDLE l_r_event = NULL;
static HANDLE l_w_event = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/**
 * rproc_type_release() - release a remote processor instance
 * @dev: the rproc's device
 *
 * This function should _never_ be called directly.
 *
 * It will be called by the driver core when no one holds a valid pointer
 * to @dev anymore.
 */
static void rproc_type_release(struct device *dev)
{
	struct rproc *rproc = container_of(dev, struct rproc, dev);

	dev_info(&rproc->dev, "releasing %s\n", rproc->name);

	//rproc_delete_debug_dir(rproc);

	//idr_destroy(&rproc->notifyids);

//	if (rproc->index >= 0)
//		ida_simple_remove(&rproc_dev_index, rproc->index);

	kfree(rproc);
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
struct rproc *rproc_alloc(struct device *dev, const char *name,
				const struct rproc_ops *ops,
				const char *firmware, int len)
{
	struct rproc *rproc;
	char *p, *template = "rproc-%s-fw";
	int name_len = 0;

	if (!dev || !name || !ops)
		return NULL;

	if (!firmware)
		/*
		 * Make room for default firmware name (minus %s plus '\0').
		 * If the caller didn't pass in a firmware name then
		 * construct a default name.  We're already glomming 'len'
		 * bytes onto the end of the struct rproc allocation, so do
		 * a few more for the default firmware name (but only if
		 * the caller doesn't pass one).
		 */
		name_len = strlen(name) + strlen(template) - 2 + 1;

	rproc = kzalloc(sizeof(struct rproc) + len + name_len, GFP_KERNEL);
	if (!rproc) {
		dev_err(dev, "%s: kzalloc failed\n", __func__);
		return NULL;
	}

	if (!firmware) {
		p = (char *)rproc + sizeof(struct rproc) + len;
		snprintf(p, name_len, template, name);
	} else {
		p = (char *)firmware;
	}

	rproc->firmware = p;
	rproc->name = name;
	rproc->ops = ops;
	rproc->priv = &rproc[1];

//	device_initialize(&rproc->dev);
	rproc->dev.parent = dev;
	rproc->dev.type = &rproc_type;

	/* Assign a unique device index and name */
//	rproc->index = ida_simple_get(&rproc_dev_index, 0, 0, GFP_KERNEL);
//	if (rproc->index < 0) {
//		dev_err(dev, "ida_simple_get failed: %d\n", rproc->index);
//		put_device(&rproc->dev);
//		return NULL;
//	}

//	dev_set_name(&rproc->dev, "remoteproc%d", rproc->index);

	atomic_set(&rproc->power, 0);

	/* Set ELF as the default fw_ops handler */
//	rproc->fw_ops = &rproc_elf_fw_ops;

//	mutex_init(&rproc->lock);

//	idr_init(&rproc->notifyids);

//	INIT_LIST_HEAD(&rproc->carveouts);
//	INIT_LIST_HEAD(&rproc->mappings);
//	INIT_LIST_HEAD(&rproc->traces);
//	INIT_LIST_HEAD(&rproc->rvdevs);

//	INIT_WORK(&rproc->crash_handler, rproc_crash_handler_work);
//	init_completion(&rproc->crash_comp);

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
