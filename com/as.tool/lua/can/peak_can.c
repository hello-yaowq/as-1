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
#include "Std_Types.h"
#include "lascanlib.h"
#include <sys/queue.h>
#include <pthread.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
struct Can_PeakHandle_s
{
	uint32_t busid;
	uint32_t port;
	uint32_t baudrate;
	can_device_rx_notification_t rx_notification;
	STAILQ_ENTRY(Can_PeakHandle_s) entry;
};
struct Can_PeakHandleList_s
{
	pthread_t rx_thread;
	volatile boolean   terminated;
	STAILQ_HEAD(,Can_PeakHandle_s) head;
};
/* ============================ [ DECLARES  ] ====================================================== */
static boolean peak_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification);
static boolean peak_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data);
static void peak_close(uint32_t port);
static void * rx_daemon(void *);
/* ============================ [ DATAS     ] ====================================================== */
const Can_DeviceOpsType can_peak_ops =
{
	.name = "peak",
	.probe = peak_probe,
	.close = peak_close,
	.write = peak_write,
};
static struct Can_PeakHandleList_s* peakH = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
static struct Can_PeakHandle_s* getHandle(uint32_t port)
{
	struct Can_PeakHandle_s *handle,*h;
	handle = NULL;
	if(NULL != peakH)
	{
		STAILQ_FOREACH(h,&peakH->head,entry)
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
static boolean peak_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification)
{
	boolean rv = TRUE;;
	struct Can_PeakHandle_s* handle;
	if(NULL == peakH)
	{
		peakH = malloc(sizeof(struct Can_PeakHandleList_s));
		asAssert(peakH);
		STAILQ_INIT(&peakH->head);

		peakH->terminated = TRUE;
	}

	if(TRUE == peakH->terminated)
	{
		if( 0 == pthread_create(&(peakH->rx_thread),NULL,rx_daemon,NULL))
		{
			peakH->terminated = FALSE;
		}
		else
		{
			asAssert(0);
		}
	}
	handle = getHandle(port);

	if(handle)
	{
		ASWARNING("CAN PEAK port=%d is already on-line, no need to probe it again!\n",port);
		rv = FALSE;
	}
	else
	{
		if( 0 )
		{	/* open port OK */
			handle = malloc(sizeof(struct Can_PeakHandle_s));
			asAssert(handle);
			handle->busid = busid;
			handle->port = port;
			handle->baudrate = baudrate;
			handle->rx_notification = rx_notification;
			STAILQ_INSERT_TAIL(&peakH->head,handle,entry);
		}
		else
		{
			ASWARNING("CAN PEAK port=%d is is not able to be opened!\n",port);
			rv = FALSE;
		}
	}

	return rv;
}
static boolean peak_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data)
{
	boolean rv = TRUE;
	struct Can_PeakHandle_s* handle = getHandle(port);
	if(handle != NULL)
	{
		if( 0 )
		{
			/* send OK */
		}
		else
		{
			rv = FALSE;
			ASWARNING("CAN PEAK port=%d send message failed!\n",port);
		}
	}
	else
	{
		rv = FALSE;
		ASWARNING("CAN Peak port=%d is not on-line, not able to send message!\n",port);
	}

	return rv;
}
static void peak_close(uint32_t port)
{
	struct Can_PeakHandle_s* handle = getHandle(port);
	if(NULL != handle)
	{
		STAILQ_REMOVE(&peakH->head,handle,Can_PeakHandle_s,entry);

		free(handle);

		if(FALSE == STAILQ_EMPTY(&peakH->head))
		{
			peakH->terminated = TRUE;
		}
	}
}

static void rx_notifiy(struct Can_PeakHandle_s* handle)
{

}
static void * rx_daemon(void * param)
{
	(void)param;
	struct Can_PeakHandle_s* handle;
	while(FALSE == peakH->terminated)
	{
		STAILQ_FOREACH(handle,&peakH->head,entry)
		{
		}
	}

	return NULL;
}

/* ============================ [ FUNCTIONS ] ====================================================== */


