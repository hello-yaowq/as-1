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
typedef struct
{
	const RPmsg_ConfigType* config;
	boolean online;
	boolean initialized;
}rpmsg_t;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static rpmsg_t rpmsg =
{
	.initialized = FALSE
};
/* ============================ [ LOCALS    ] ====================================================== */
static Std_ReturnType sendMessage(uint32 dstEndpt, uint32 srcEndpt, void* data, uint16 len)
{
	Std_ReturnType ercd;
	VirtQ_IdxType idx;
	RPmsg_HandlerType* msg;
	uint16 length;
	ASLOG(RPMSG,"RPmsg send(dst=%Xh,src=%Xh,data=%Xh,len=%d)\n",dstEndpt,srcEndpt,data,len);
	ercd = VirtQ_GetAvailiableBuffer(rpmsg.config->txChl,&idx,(void**)&msg,&length);
	if(E_OK == ercd)
	{
		if(len > length){
			len = length;
		}
		/* Copy the payload and set message header: */
		memcpy(msg->data, data, len);
		msg->len = len;
		msg->dst = dstEndpt;
		msg->src = srcEndpt;
		msg->flags = 0;
		msg->reserved = 0;

		VirtQ_AddUsedBuffer(rpmsg.config->txChl, idx, len);
		VirtQ_Kick(rpmsg.config->txChl);
	}
	return ercd;
}
static void sendNamseServiceMessage(char * name, uint32 port, RPmsg_NameServiceFlagType flags)
{
	RPmsg_NamseServiceMessageType nsMsg;
    Std_ReturnType ercd;

    strncpy(nsMsg.name, name, RPMSG_NAME_SIZE);
    nsMsg.addr = port;
    nsMsg.flags = flags;

    ASLOG(RPMSG,"RPmsg create <%s> on port=0x%X\n",name,port);
    ercd = sendMessage(RPMSG_NAME_SERVICE_PORT, port, &nsMsg, sizeof(nsMsg));
    assert(E_OK == ercd);
}

static void NameSerivice_Create(char* name, uint32 port)
{
	sendNamseServiceMessage(name, port, RPMSG_NS_CREATE);
}

static void NameSerivice_Destroy(char * name, uint32 port)
{
	sendNamseServiceMessage(name, port, RPMSG_NS_DESTROY);
}

/* ============================ [ FUNCTIONS ] ====================================================== */
void RPmsg_Init(const RPmsg_ConfigType* config)
{
	if(FALSE == rpmsg.initialized)
	{
		rpmsg.initialized = TRUE;
		rpmsg.online = FALSE;
		rpmsg.config = config;
	}
	else
	{
		assert(0);
	}
}
void RPmsg_RxNotification(VirtQ_ChannerlType channel)
{
	assert(rpmsg.initialized);
	assert(channel == rpmsg.config->rxChl);
	if(rpmsg.online)
	{

	}
	else
	{
		ASLOG(RPMSG,"RPmsg_RxNotification(offline-->online)\n");
		VirtQ_InitVq(rpmsg.config->rxChl);
		VirtQ_InitVq(rpmsg.config->txChl);
		NameSerivice_Create(rpmsg.config->name,rpmsg.config->port);
		rpmsg.online = TRUE;
	}
}
void RPmsg_TxConfirmation(VirtQ_ChannerlType channel)
{

}

Std_ReturnType RPmsg_Send(RPmsg_ChannelType chl, void* data, uint16 len)
{

	return E_OK;
}
