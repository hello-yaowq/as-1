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
#include "rs232.h"
#include "Std_Types.h"
#include "lascanlib.h"
#include <sys/queue.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
struct Can_SerialHandle_s
{
	uint32_t busid;
	uint32_t port;
	uint32_t baudrate;
	can_device_rx_notification_t rx_notification;
	STAILQ_ENTRY(Can_SerialHandle_s) entry;
};
struct Can_SerialHandleList_s
{
	STAILQ_HEAD(,Can_SerialHandle_s) head;
};
/* ============================ [ DECLARES  ] ====================================================== */
static boolean serial_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification);
static boolean serial_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data);
static void serial_close(uint32_t port);
/* ============================ [ DATAS     ] ====================================================== */
const Can_DeviceOpsType can_serial_ops =
{
	.name = "serial",
	.probe = serial_probe,
	.close = serial_close,
	.write = serial_write,
};
static struct Can_SerialHandleList_s* serialH = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
static struct Can_SerialHandle_s* getHandle(uint32_t port)
{
	struct Can_SerialHandle_s *handle,*h;
	handle = NULL;
	if(NULL != serialH)
	{
		STAILQ_FOREACH(h,&serialH->head,entry)
		{
			if(h->port == port)
			{
				handle = h;
				break;
			}
		}
	}
	return handle;
}
static boolean serial_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification)
{
	boolean rv;
	struct Can_SerialHandle_s* handle;
	if(NULL == serialH)
	{
		serialH = malloc(sizeof(struct Can_SerialHandleList_s));
		asAssert(serialH);
		STAILQ_INIT(&serialH->head);
	}
	handle = getHandle(port);

	if(handle)
	{
		ASWARNING("CAN SERIAL port=%d is already on-line, no need to probe it again!\n",port);
		rv = FALSE;
	}
	else
	{
		handle = malloc(sizeof(struct Can_SerialHandle_s));
		STAILQ_INSERT_TAIL(&serialH->head,handle,entry);
		handle->busid = busid;
		handle->port = port;
		handle->baudrate = baudrate;
		handle->rx_notification = rx_notification;
		rv = TRUE;
	}

	return rv;
}
static boolean serial_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data)
{
	boolean rv = TRUE;

	if(TRUE)
	{

	}
	else
	{
		rv = FALSE;
		ASWARNING("CAN RPMSG port=%d is not on-line, not able to send message!\n",port);
	}

	return rv;
}
static void serial_close(uint32_t port)
{
	struct Can_SerialHandle_s* handle = getHandle(port);
	if(NULL != handle)
	{
		STAILQ_REMOVE(&serialH->head,handle,Can_SerialHandle_s,entry);
		free(handle);
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */


