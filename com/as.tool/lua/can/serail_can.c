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
#include "RPmsg.h"
#include "lascanlib.h"
#include <sys/queue.h>
#include <pthread.h>
/* ============================ [ MACROS    ] ====================================================== */
#define CAN_SERIAL_CACHE_SIZE  1024
/* ============================ [ TYPES     ] ====================================================== */
struct Can_SerialHandle_s
{
	uint32_t busid;
	uint32_t port;
	uint32_t baudrate;
	char cache[CAN_SERIAL_CACHE_SIZE];
	int r_pos;
	int w_pos;
	can_device_rx_notification_t rx_notification;
	STAILQ_ENTRY(Can_SerialHandle_s) entry;
};
struct Can_SerialHandleList_s
{
	pthread_t rx_thread;
	volatile boolean   terminated;
	STAILQ_HEAD(,Can_SerialHandle_s) head;
};
typedef struct {
    /* the CAN ID, 29 or 11-bit */
	uint32_t    magic;
    uint32_t 	id;
    uint8_t     bus;
    /* Length, max 8 bytes */
    uint8_t		length;
    /* data ptr */
    uint8_t 		sdu[8];
} Can_SerialPduType;
/* ============================ [ DECLARES  ] ====================================================== */
static boolean serial_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification);
static boolean serial_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data);
static void serial_close(uint32_t port);
static void * rx_daemon(void *);
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
	boolean rv = TRUE;;
	struct Can_SerialHandle_s* handle;
	if(NULL == serialH)
	{
		serialH = malloc(sizeof(struct Can_SerialHandleList_s));
		asAssert(serialH);
		STAILQ_INIT(&serialH->head);

		if( 0 == pthread_create(&(serialH->rx_thread),NULL,rx_daemon,NULL))
		{
			serialH->terminated = FALSE;
		}
		else
		{
			asAssert(0);
		}
	}
	handle = getHandle(port);

	if(handle)
	{
		ASWARNING("CAN SERIAL port=%d is already on-line, no need to probe it again!\n",port);
		rv = FALSE;
	}
	else
	{
		if( 0 == RS232_OpenComport(port,baudrate,"8N1"))
		{	/* open port OK */
			handle = malloc(sizeof(struct Can_SerialHandle_s));
			asAssert(handle);
			handle->busid = busid;
			handle->port = port;
			handle->baudrate = baudrate;
			handle->rx_notification = rx_notification;
			STAILQ_INSERT_TAIL(&serialH->head,handle,entry);
		}
		else
		{
			rv = FALSE;
		}
	}

	return rv;
}
static boolean serial_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data)
{
	boolean rv = TRUE;
	struct Can_SerialHandle_s* handle = getHandle(port);
	if(handle != NULL)
	{
		Can_SerialPduType pduInfo;
		pduInfo.magic = ((uint32)'S'<<24) + ((uint32)'C'<<16) + ((uint32)'A'<<8) + ((uint32)'N'<<0);
		pduInfo.bus = port;
		pduInfo.id = canid;
		pduInfo.length = dlc;
		memcpy(pduInfo.sdu,data,dlc);

		if(0 == RS232_SendBuf((int)handle->port,(unsigned char*)&pduInfo,sizeof(Can_SerialPduType)))
		{
			/* send OK */
		}
		else
		{
			rv = FALSE;
			ASWARNING("CAN Serial port=%d send message failed!\n",port);
		}
	}
	else
	{
		rv = FALSE;
		ASWARNING("CAN Serial port=%d is not on-line, not able to send message!\n",port);
	}

	return rv;
}
static void serial_close(uint32_t port)
{
	struct Can_SerialHandle_s* handle = getHandle(port);
	if(NULL != handle)
	{
		STAILQ_REMOVE(&serialH->head,handle,Can_SerialHandle_s,entry);

		RS232_CloseComport(handle->port);

		free(handle);

		if(FALSE == STAILQ_EMPTY(&serialH->head))
		{
			serialH->terminated = TRUE;
		}
	}
}
static void * rx_daemon(void * param)
{
	(void)param;
	struct Can_SerialHandle_s* handle;
	while(FALSE == serialH->terminated)
	{
		STAILQ_FOREACH(handle,&serialH->head,entry)
		{
			RS232_PollComport()
		}
	}

	return NULL;
}

/* ============================ [ FUNCTIONS ] ====================================================== */


