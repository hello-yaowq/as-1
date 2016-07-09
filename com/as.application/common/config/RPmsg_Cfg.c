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
#include "RPmsg.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
void __weak Can_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len){}
void __weak Can_RPmsg_TxConfirmation(RPmsg_ChannelType chl) {}
#ifdef USE_SHELL
void Shell_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len);
void Shell_RPmsg_TxConfirmation(RPmsg_ChannelType chl);
#endif
/* ============================ [ DATAS     ] ====================================================== */

static const RPmsg_PortConfigType portConfig[RPMSG_PORT_NUM] =
{
	{
		.name = "RPMSG-SAMPLE",
		.port = 0x257,
		.rxChl = VIRTQ_CHL_RPMSG_RX,
		.txChl = VIRTQ_CHL_RPMSG_TX,
	}
};
static const RPmsg_ChannelConfigType chlConfig[RPMSG_CHL_NUM] =
{
#ifdef USE_SHELL
	{
		.dst = 0xCAD,
		.rxNotification = Shell_RPmsg_RxNotitication,
		.txConfirmation = Shell_RPmsg_TxConfirmation,
		.portConfig = &portConfig[RPMSG_PORT_DEFAULT]
	},
#endif
	{
		.dst = 0xCAB,
		.rxNotification = Can_RPmsg_RxNotitication,
		.txConfirmation = Can_RPmsg_TxConfirmation,
		.portConfig = &portConfig[RPMSG_PORT_DEFAULT]
	}
};
const RPmsg_ConfigType RPmsg_Config =
{
	.portConfig = portConfig,
	.chlConfig = chlConfig
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
