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
#include "VirtQ.h"
#include "Ipc.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
DECLARE_WEAK void RPmsg_RxNotification(VirtQ_ChannerlType channel){}
DECLARE_WEAK void RPmsg_TxConfirmation(VirtQ_ChannerlType channel){}
/* ============================ [ DATAS     ] ====================================================== */
static const VirtQ_QueueConfigType queueConfig[VIRTQ_CHL_NUM] =
{
	{ .idx = VIRTQ_CHL_RPMSG_RX, .rxNotification = RPmsg_RxNotification },
	{ .idx = VIRTQ_CHL_RPMSG_TX, .rxNotification = RPmsg_TxConfirmation }
};
const VirtQ_ConfigType VirtQ_Config =
{
	.queueConfig = queueConfig
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
