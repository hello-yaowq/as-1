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
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/platform_device.h>
#include "core.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

static int asvirt_pctl_probe(struct platform_device *pdev)
{
	printk("aslinux virt-gpio -probed\n");
	return 0;
}

static struct of_device_id asvirt_pctl_of_match[] = {
	{ .compatible = "as,virtual-pinctrl", .data = NULL },
};
static struct platform_driver asvirt_pctl_driver = {
	.driver = {
		.name = "asvirt-pinctrl",
		.owner = THIS_MODULE,
		.of_match_table = asvirt_pctl_of_match,
	},
	.probe = asvirt_pctl_probe,
};

static int __init asvirt_pctl_init(void)
{
	printk("\n\n >> register ASLINUX VIRTUAL GPIO driver \n\n");
	return platform_driver_register(&asvirt_pctl_driver);
}
subsys_initcall(asvirt_pctl_init);
