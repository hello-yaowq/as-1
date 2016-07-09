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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include "remoteproc_internal.h"

/* ============================ [ MACROS    ] ====================================================== */
#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec) \
    do { \
        long timeout = (nMilliSec) * HZ /1000; \
        while (timeout > 0) \
        { \
            timeout = schedule_timeout(timeout); \
        } \
    }while (0);
#endif
/* ============================ [ TYPES     ] ====================================================== */

/*
 * struct asvirt_rproc_pdata - asvirt remoteproc's platform data
 * @name: the remoteproc's name
 * @oh_name: asvirt hwmod device
 * @oh_name_opt: optional, secondary asvirt hwmod device
 * @firmware: name of firmware file to load
 * @mbox_name: name of asvirt mailbox device to use with this rproc
 * @ops: start/stop rproc handlers
 * @device_enable: asvirt-specific handler for enabling a device
 * @device_shutdown: asvirt-specific handler for shutting down a device
 * @set_bootaddr: asvirt-specific handler for setting the rproc boot address
 */
struct asvirt_rproc_pdata {
	const char *name;
	const char *oh_name;
	const char *oh_name_opt;
	const char *firmware;
	const char *mbox_name;
	const struct rproc_ops *ops;
	int (*device_enable) (struct platform_device *pdev);
	int (*device_shutdown) (struct platform_device *pdev);
	void(*set_bootaddr)(u32);
};

/**
 * struct asvirt_rproc - asvirt remote processor state
 * @rproc: rproc handle
 */
struct asvirt_rproc {
	struct task_struct * task_linux;
	struct task_struct * task_mcu;

	/* r and w to linux */
	struct mutex r_lock;
	struct mutex w_lock;
	struct semaphore r_event;
	struct semaphore w_event;

	struct rproc *rproc;
};
/* ============================ [ DECLARES  ] ====================================================== */
static int asvirt_rproc_probe(struct platform_device *pdev);
static int asvirt_rproc_remove(struct platform_device *pdev);
static void asvirt_rproc_kick(struct rproc *rproc, int vqid);
static int asvirt_rproc_stop(struct rproc *rproc);
static int asvirt_rproc_start(struct rproc *rproc);
/* ============================ [ DATAS     ] ====================================================== */
static struct rproc_ops asvirt_rproc_ops = {
	.start		= asvirt_rproc_start,
	.stop		= asvirt_rproc_stop,
	.kick		= asvirt_rproc_kick,
};


static struct of_device_id asvirt_rproc_of_match[] = {
	{ .compatible = "as,virtual-rproc" },
};

static struct platform_driver asvirt_rproc_driver = {
	.probe = asvirt_rproc_probe,
	.remove = asvirt_rproc_remove,
	.driver = {
		.name = "asvirt-rproc",
		.owner = THIS_MODULE,
		.of_match_table = asvirt_rproc_of_match,
	},
};
/* ============================ [ LOCALS    ] ====================================================== */

/* kick a virtqueue */
static void asvirt_rproc_kick(struct rproc *rproc, int vqid)
{
	struct asvirt_rproc *oproc = rproc->priv;
	struct device *dev = rproc->dev.parent;
	int ret;

	/* send the index of the triggered virtqueue in the mailbox payload */
	ret = 0;
	if (ret)
		dev_err(dev, "asvirt_mbox_msg_send failed: %d\n", ret);
}

/*
 * Power up the remote processor.
 *
 * This function will be invoked only after the firmware for this rproc
 * was loaded, parsed successfully, and all of its resource requirements
 * were met.
 */
static int asvirt_rproc_start(struct rproc *rproc)
{
	struct asvirt_rproc *oproc = rproc->priv;
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct asvirt_rproc_pdata *pdata = pdev->dev.platform_data;
	int ret;

	if (pdata->set_bootaddr)
		pdata->set_bootaddr(rproc->bootaddr);

	/*
	 * Ping the remote processor. this is only for sanity-sake;
	 * there is no functional effect whatsoever.
	 *
	 * Note that the reply will _not_ arrive immediately: this message
	 * will wait in the mailbox fifo until the remote processor is booted.
	 */
	ret = 0;
	if (ret) {
		dev_err(dev, "asvirt_mbox_get failed: %d\n", ret);
		goto put_mbox;
	}

	ret = pdata->device_enable(pdev);
	if (ret) {
		dev_err(dev, "asvirt_device_enable failed: %d\n", ret);
		goto put_mbox;
	}

	return 0;

put_mbox:

	return ret;
}

/* power off the remote processor */
static int asvirt_rproc_stop(struct rproc *rproc)
{
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct asvirt_rproc_pdata *pdata = pdev->dev.platform_data;
	struct asvirt_rproc *oproc = rproc->priv;
	int ret;

	ret = pdata->device_shutdown(pdev);
	if (ret)
		return ret;

	return 0;
}

static int thread_rproc_linux(void *data)
{
	struct rproc *rproc =  (struct rproc *)data;
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct asvirt_rproc *oproc = rproc->priv;

	/* sleep for a while make sure MCU thread run firstly */
	SLEEP_MILLI_SEC(100);
	dev_info(&pdev->dev, "rproc linux side thread is running...\n");

	while(1)
	{
		down(&oproc->r_event);

		mutex_lock(&oproc->r_lock);

		dev_info(&pdev->dev, "^_^ rproc linux side thread get the semaphore...\n");

		mutex_unlock(&oproc->r_lock);

	}

	return 0;
}

static int thread_rproc_mcu(void *data)
{
	struct rproc *rproc =  (struct rproc *)data;
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct asvirt_rproc *oproc = rproc->priv;

	dev_info(&pdev->dev, "rproc MCU side thread is running...\n");

	while(1)
	{

		/* fall asleep for one second */
		SLEEP_MILLI_SEC(1000);

		mutex_lock(&oproc->r_lock);

		dev_info(&pdev->dev, "^_^ rproc MCU side thread give semaphore...\n");

		up(&oproc->r_event);

		mutex_unlock(&oproc->r_lock);
	}

	return 0;
}

static int asvirt_rproc_probe(struct platform_device *pdev)
{
	const char* firmware_name;
	struct device_node *np = pdev->dev.of_node;
	struct asvirt_rproc *oproc;
	struct rproc *rproc;
	int ret;

	ret = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(&pdev->dev, "dma_set_coherent_mask: %d\n", ret);
		return ret;
	}

	ret = of_property_read_string(np,"fwname",&firmware_name);
	if (ret) {
		dev_err(&pdev->dev, "of_property_read_string: %d\n", ret);
		return ret;
	}

	rproc = rproc_alloc(&pdev->dev, firmware_name, &asvirt_rproc_ops,
				NULL, sizeof(*oproc));
	if (!rproc)
		return -ENOMEM;

	oproc = rproc->priv;
	oproc->rproc = rproc;

	platform_set_drvdata(pdev, rproc);

	ret = rproc_add(rproc);
	if (ret)
		goto free_rproc;

	oproc->task_linux = kthread_run(&thread_rproc_linux,(void *)rproc,"rproc-linux");
	if(NULL == oproc->task_linux)
		goto free_rproc;

	oproc->task_mcu = kthread_run(&thread_rproc_mcu,(void *)rproc,"rproc-mcu");
	if(NULL == oproc->task_mcu)
		goto free_rproc;

	mutex_init(&oproc->r_lock);
	mutex_init(&oproc->w_lock);

	sema_init(&oproc->r_event,0);
	sema_init(&oproc->w_event,0);

	dev_info(&pdev->dev, "initialized ASVIRT remote processor driver\n");

	return 0;

free_rproc:
	rproc_put(rproc);
	return ret;
}

static int asvirt_rproc_remove(struct platform_device *pdev)
{
	struct rproc *rproc = platform_get_drvdata(pdev);
	struct asvirt_rproc *oproc = rproc->priv;

	rproc_del(rproc);
	rproc_put(rproc);

	kthread_stop(oproc->task_linux);
	kthread_stop(oproc->task_mcu);

	return 0;
}
/* ============================ [ FUNCTIONS ] ====================================================== */


module_platform_driver(asvirt_rproc_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ASVIRT Remote Processor control driver");
