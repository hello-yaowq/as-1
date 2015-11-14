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
static Std_ReturnType sendMessage(const RPmsg_PortConfigType* portConfig, uint32 dstEndpt, uint32 srcEndpt, void* data, uint16 len)
{
	Std_ReturnType ercd;
	VirtQ_IdxType idx;
	RPmsg_HandlerType* msg;
	uint16 length;
	ASLOG(OFF,"RPmsg send(dst=%Xh,src=%Xh,data=%Xh,len=%d)\n",dstEndpt,srcEndpt,data,len);
	ercd = VirtQ_GetAvailiableBuffer(portConfig->txChl,&idx,(void**)&msg,&length);
	if(E_OK == ercd)
	{
		if(len > length){
			len = length;
			ASLOG(RPMSG,"warning transmit message length=%d > buffer length=%d, truncate.\n",len,length);
		}
		/* Copy the payload and set message header: */
		memcpy(msg->data, data, len);
		msg->len = len;
		msg->dst = dstEndpt;
		msg->src = srcEndpt;
		msg->flags = 0;
		msg->reserved = 0;

		VirtQ_AddUsedBuffer(portConfig->txChl, idx, len);
		VirtQ_Kick(portConfig->txChl);
	}
	return ercd;
}
static void sendNamseServiceMessage(const RPmsg_PortConfigType* portConfig, RPmsg_NameServiceFlagType flags)
{
	RPmsg_NamseServiceMessageType nsMsg;
    Std_ReturnType ercd;

    strncpy(nsMsg.name, portConfig->name, RPMSG_NAME_SIZE);
    nsMsg.addr = portConfig->port;
    nsMsg.flags = flags;

    ASLOG(OFF,"RPmsg create <%s> on port=0x%X\n",portConfig->name,portConfig->port);
    ercd = sendMessage(portConfig,RPMSG_NAME_SERVICE_PORT, portConfig->port, &nsMsg, sizeof(nsMsg));
    asAssert(E_OK == ercd);
}

static void NameSerivice_Create(const RPmsg_PortConfigType* portConfig)
{
	sendNamseServiceMessage(portConfig, RPMSG_NS_CREATE);
}
#if 0
static void NameSerivice_Destroy(char * name, uint32 port)
{
	sendNamseServiceMessage(name, port, RPMSG_NS_DESTROY);
}
#endif
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
		asAssert(0);
	}
}
void RPmsg_RxNotification(RPmsg_PortType port)
{
	VirtQ_IdxType idx;
	uint16_t length;
	RPmsg_HandlerType* msg;
	Std_ReturnType ercd;
	RPmsg_ChannelType chl;
	const RPmsg_PortConfigType* portConfig;
	asAssert(rpmsg.initialized);
	asAssert(port < RPMSG_PORT_NUM);

	portConfig = &(rpmsg.config->portConfig[port]);
	if(rpmsg.online)
	{
		ercd = VirtQ_GetAvailiableBuffer(portConfig->rxChl,&idx,(void**)&msg,&length);
		if(E_OK == ercd)
		{
			ASLOG(OFF,"RPmsg rx(dst=%Xh,src=%Xh,data=%Xh,len=%d/%d)\n",msg->dst,msg->src,msg->data,msg->len,length);
			for(chl=0;chl<RPMSG_CHL_NUM;chl++)
			{
				if( (portConfig==rpmsg.config->chlConfig[chl].portConfig) &&
					(msg->dst==rpmsg.config->chlConfig[chl].dst) )
				{
					break;
				}
			}

			if(chl<RPMSG_CHL_NUM)
			{
				rpmsg.config->chlConfig[chl].rxNotification(chl,msg->data,msg->len);
			}
			else
			{
				asAssert(0);
			}

			VirtQ_AddUsedBuffer(portConfig->rxChl, idx, length);
			VirtQ_Kick(portConfig->rxChl);
		}
		else
		{
			asAssert(0);
			ASLOG(OFF,"invalid RPmsg_RxNotification\n");
		}
	}
	else
	{
		ASLOG(RPMSG,"RPmsg_RxNotification(offline-->online)\n");
		VirtQ_InitVq(portConfig->rxChl);
		VirtQ_InitVq(portConfig->txChl);
		NameSerivice_Create(portConfig);
		rpmsg.online = TRUE;
	}
}

boolean RPmsg_IsOnline(void)
{
	return rpmsg.online;
}
void RPmsg_TxConfirmation(RPmsg_PortType channel)
{

}

Std_ReturnType RPmsg_Send(RPmsg_ChannelType chl, void* data, uint16 len)
{
	Std_ReturnType ercd;
	const RPmsg_ChannelConfigType* chlConfig;
	asAssert(rpmsg.initialized);
	asAssert(chl<RPMSG_CHL_NUM);
	chlConfig=&(rpmsg.config->chlConfig[chl]);
	ercd = sendMessage(chlConfig->portConfig,chlConfig->dst,chlConfig->portConfig->port,data,len);
	return ercd;
}
