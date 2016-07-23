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
#include <linux/rpmsg.h>
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/if_arp.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/types.h>

#include <linux/can/dev.h>
#include <linux/can/error.h>
#include <linux/can/led.h>
/* ============================ [ MACROS    ] ====================================================== */
#define MSG		"can:hello world!"
#define MSG_LIMIT	100
/* ============================ [ TYPES     ] ====================================================== */

/**
 * struct ascan_priv - This definition define CAN driver instance
 * @can:			CAN private data structure.
 * @tx_head:			Tx CAN packets ready to send on the queue
 * @tx_tail:			Tx CAN packets successfully sended on the queue
 * @tx_max:			Maximum number packets the driver can send
 * @napi:			NAPI structure
 * @read_reg:			For reading data from CAN registers
 * @write_reg:			For writing data to CAN registers
 * @dev:			Network device data structure
 * @reg_base:			Ioremapped address to registers
 * @irq_flags:			For request_irq()
 * @bus_clk:			Pointer to struct clk
 * @can_clk:			Pointer to struct clk
 */
struct ascan_priv {
	struct can_priv can;
	struct napi_struct napi;
	struct net_device *dev;

	struct rpmsg_channel *rpdev;
};
/* ============================ [ DECLARES  ] ====================================================== */
static int rpmsg_can_probe(struct rpmsg_channel *rpdev);
static void rpmsg_can_cb(struct rpmsg_channel *rpdev, void *data, int len,
						void *priv, u32 src);
static void rpmsg_can_remove(struct rpmsg_channel *rpdev);

static int ascan_open(struct net_device *ndev);
static int ascan_close(struct net_device *ndev);
static int ascan_start_xmit(struct sk_buff *skb, struct net_device *ndev);
/* ============================ [ DATAS     ] ====================================================== */
static const struct net_device_ops ascan_netdev_ops = {
	.ndo_open	= ascan_open,
	.ndo_stop	= ascan_close,
	.ndo_start_xmit	= ascan_start_xmit,
	.ndo_change_mtu	= can_change_mtu,
};

static struct rpmsg_device_id rpmsg_driver_can_id_table[] = {
	{ .name	= "rpmsg-client-sample" },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_driver_can_id_table);
static struct rpmsg_driver rpmsg_can_client = {
	.drv.name	= KBUILD_MODNAME,
	.drv.owner	= THIS_MODULE,
	.id_table	= rpmsg_driver_can_id_table,
	.probe		= rpmsg_can_probe,
	.callback	= rpmsg_can_cb,
	.remove		= rpmsg_can_remove,
};
/* ============================ [ LOCALS    ] ====================================================== */

/**
 * ascan_open - Driver open routine
 * @ndev:	Pointer to net_device structure
 *
 * This is the driver open routine.
 * Return: 0 on success and failure value on error
 */
static int ascan_open(struct net_device *ndev)
{
	struct ascan_priv *priv = netdev_priv(ndev);
	int ret;

	/* Common open */
	ret = open_candev(ndev);
	if (ret)
		return ret;

	can_led_event(ndev, CAN_LED_EVENT_OPEN);
	napi_enable(&priv->napi);
	netif_start_queue(ndev);

	return 0;
}

/**
 * ascan_close - Driver close routine
 * @ndev:	Pointer to net_device structure
 *
 * Return: 0 always
 */
static int ascan_close(struct net_device *ndev)
{
	struct ascan_priv *priv = netdev_priv(ndev);

	netif_stop_queue(ndev);
	napi_disable(&priv->napi);
	close_candev(ndev);

	can_led_event(ndev, CAN_LED_EVENT_STOP);

	return 0;
}

/**
 * ascan_start_xmit - Starts the transmission
 * @skb:	sk_buff pointer that contains data to be Txed
 * @ndev:	Pointer to net_device structure
 *
 * This function is invoked from upper layers to initiate transmission. This
 * function uses the next available free txbuff and populates their fields to
 * start the transmission.
 *
 * Return: 0 on success and failure value on error
 */
static int ascan_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	int ret;
	struct ascan_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	struct can_frame *cf = (struct can_frame *)skb->data;
	struct rpmsg_channel *rpdev = priv->rpdev;

	if (can_dropped_invalid_skb(ndev, skb))
		return NETDEV_TX_OK;

	/* send a new message now */
	ret = rpmsg_send(rpdev, cf, sizeof(*cf));
	if (ret)
	{
		dev_err(&rpdev->dev, "CAN: rpmsg_send failed: %d\n", ret);

		return ret;
	}


	stats->tx_bytes += cf->can_dlc;


	return NETDEV_TX_OK;
}

static void rpmsg_can_cb(struct rpmsg_channel *rpdev, void *data, int len,
						void *priv, u32 src)
{
	int ret;
	static int rx_count;

	dev_info(&rpdev->dev, "incoming msg %d (src: 0x%x)\n", ++rx_count, src);

	print_hex_dump(KERN_DEBUG, __func__, DUMP_PREFIX_NONE, 16, 1,
		       data, len,  true);

	/* samples should not live forever */
	if (rx_count >= MSG_LIMIT) {
		dev_info(&rpdev->dev, "goodbye!\n");
		return;
	}

	/* send a new message now */
	ret = rpmsg_send(rpdev, MSG, strlen(MSG));
	if (ret)
		dev_err(&rpdev->dev, "CAN:rpmsg_send failed: %d\n", ret);
}

static int rpmsg_can_probe(struct rpmsg_channel *rpdev)
{
	int ret;

	struct net_device *ndev;
	struct ascan_priv *priv;

	dev_info(&rpdev->dev, "new channel: 0x%x -> 0x%x!\n",
					rpdev->src, rpdev->dst);

	/* Create a CAN device instance */
	ndev = alloc_candev(sizeof(struct ascan_priv), 0x40);
	if (!ndev)
		return -ENOMEM;

	priv = netdev_priv(ndev);
	priv->dev = ndev;
	priv->rpdev = rpdev;

	ndev->netdev_ops = &ascan_netdev_ops;

	ret = register_candev(ndev);
	if (ret) {
		dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", ret);
		return ret;
	}

	devm_can_led_init(ndev);

	/* send a message to our remote processor */
	ret = rpmsg_send(rpdev, MSG, strlen(MSG));
	if (ret) {
		dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", ret);
		return ret;
	}

	return 0;
}

static void rpmsg_can_remove(struct rpmsg_channel *rpdev)
{
	dev_info(&rpdev->dev, "rpmsg sample client driver is removed\n");
}


static int __init rpmsg_client_can_init(void)
{
	return register_rpmsg_driver(&rpmsg_can_client);
}
static void __exit rpmsg_client_can_fini(void)
{
	unregister_rpmsg_driver(&rpmsg_can_client);
}
/* ============================ [ FUNCTIONS ] ====================================================== */
module_init(rpmsg_client_can_init);
module_exit(rpmsg_client_can_fini);

MODULE_DESCRIPTION("Remote processor messaging sample client driver");
MODULE_LICENSE("GPL v2");
