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
#ifdef __WINDOWS__
/* ============================ [ INCLUDES  ] ====================================================== */
#include <sys/queue.h>
#include <pthread.h>
/* most of the code copy from https://github.com/linux-can/can-utils */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "Std_Types.h"
#include "lascanlib.h"
#include "asdebug.h"

/* Link with ws2_32.lib */
#ifndef __GNUC__
#pragma comment(lib, "Ws2_32.lib")
#else
/* -lwsock32 */
#endif
/* ============================ [ MACROS    ] ====================================================== */
#define CAN_MAX_DLEN 8
#define CAN_MTU sizeof(struct can_frame)
#define CAN_PORT_MIN  80
/* ============================ [ TYPES     ] ====================================================== */
/**
 * struct can_frame - basic CAN frame structure
 * @can_id:  CAN ID of the frame and CAN_*_FLAG flags, see canid_t definition
 * @can_dlc: frame payload length in byte (0 .. 8) aka data length code
 *           N.B. the DLC field from ISO 11898-1 Chapter 8.4.2.3 has a 1:1
 *           mapping of the 'data length code' to the real payload length
 * @data:    CAN frame payload (up to 8 byte)
 */
struct can_frame {
	uint32_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	uint8_t    can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
	uint8_t    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
};
struct Can_SocketHandle_s
{
	uint32_t busid;
	uint32_t port;
	uint32_t baudrate;
	can_device_rx_notification_t rx_notification;
	int s; /* can raw socket */
	struct sockaddr_in addr;
	STAILQ_ENTRY(Can_SocketHandle_s) entry;
};
struct Can_SocketHandleList_s
{
	pthread_t rx_thread;
	volatile boolean   terminated;
	STAILQ_HEAD(,Can_SocketHandle_s) head;
};
/* ============================ [ DECLARES  ] ====================================================== */
static boolean socket_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification);
static boolean socket_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data);
static void socket_close(uint32_t port);
static void * rx_daemon(void *);
/* ============================ [ DATAS     ] ====================================================== */
const Can_DeviceOpsType can_socket_ops =
{
	.name = "socket",
	.probe = socket_probe,
	.close = socket_close,
	.write = socket_write,
};
static struct Can_SocketHandleList_s* socketH = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
static struct Can_SocketHandle_s* getHandle(uint32_t port)
{
	struct Can_SocketHandle_s *handle,*h;
	handle = NULL;
	if(NULL != socketH)
	{
		STAILQ_FOREACH(h,&socketH->head,entry)
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

static boolean socket_probe(uint32_t busid,uint32_t port,uint32_t baudrate,can_device_rx_notification_t rx_notification)
{
	boolean rv = TRUE;;
	struct Can_SocketHandle_s* handle;
	if(NULL == socketH)
	{
		socketH = malloc(sizeof(struct Can_SocketHandleList_s));
		asAssert(socketH);
		STAILQ_INIT(&socketH->head);

		socketH->terminated = TRUE;

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	handle = getHandle(port);

	if(handle)
	{
		ASWARNING("CAN socket port=%d is already on-line, no need to probe it again!\n",port);
		rv = FALSE;
	}
	else
	{
		int s;
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(CAN_PORT_MIN+port);
		/* open socket */
		if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			ASWARNING("CAN Socket port=%d open failed!\n",port);
			rv = FALSE;
		}

		if( rv )
		{
			/* Connect to server. */
			int ercd = connect(s, (SOCKADDR *) & addr, sizeof (SOCKADDR));
			if (ercd == SOCKET_ERROR) {
				ASWARNING("connect function failed with error: %d\n", WSAGetLastError());
				ercd = closesocket(s);
				if (ercd == SOCKET_ERROR){
					ASWARNING("closesocket function failed with error: %d\n", WSAGetLastError());
				}
				rv = FALSE;
			}
		}

		if( rv )
		{	/* open port OK */
			handle = malloc(sizeof(struct Can_SocketHandle_s));
			asAssert(handle);
			handle->busid = busid;
			handle->port = port;
			handle->baudrate = baudrate;
			handle->rx_notification = rx_notification;
			handle->s = s;
			memcpy(&(handle->addr),&addr,sizeof(addr));
			STAILQ_INSERT_TAIL(&socketH->head,handle,entry);
		}
		else
		{
			rv = FALSE;
		}
	}

	if( (TRUE == socketH->terminated) &&
		(FALSE == STAILQ_EMPTY(&socketH->head)) )
	{
		if( 0 == pthread_create(&(socketH->rx_thread),NULL,rx_daemon,NULL))
		{
			socketH->terminated = FALSE;
		}
		else
		{
			asAssert(0);
		}
	}

	return rv;
}
static boolean socket_write(uint32_t port,uint32_t canid,uint32_t dlc,uint8_t* data)
{
	boolean rv = TRUE;
	struct Can_SocketHandle_s* handle = getHandle(port);
	if(handle != NULL)
	{
		struct can_frame frame;
		frame.can_id = canid;
		frame.can_dlc = dlc;
		memcpy(frame.data,data,dlc);

		if (send(handle->s, (const char*)&frame, CAN_MTU,0) != CAN_MTU) {
			perror("CAN socket write");
			ASWARNING("CAN Socket port=%d send message failed!\n",port);
			rv = FALSE;
		}
	}
	else
	{
		rv = FALSE;
		ASWARNING("CAN Socket port=%d is not on-line, not able to send message!\n",port);
	}

	return rv;
}
static void socket_close(uint32_t port)
{
	struct Can_SocketHandle_s* handle = getHandle(port);
	if(NULL != handle)
	{
		closesocket(handle->s);
		STAILQ_REMOVE(&socketH->head,handle,Can_SocketHandle_s,entry);

		free(handle);

		if(FALSE == STAILQ_EMPTY(&socketH->head))
		{
			socketH->terminated = TRUE;
		}
	}
}

static void rx_notifiy(struct Can_SocketHandle_s* handle)
{
	int nbytes,len;
	struct can_frame frame;
	nbytes = recvfrom(handle->s, (char*)&frame, sizeof(frame), 0, (struct sockaddr*)&handle->addr, &len);
	if (nbytes < 0) {
		perror("CAN socket read");
		ASWARNING("CAN Socket port=%d read message failed!\n",handle->port);
	}
	else
	{
		handle->rx_notification(handle->busid,frame.can_id,frame.can_dlc,frame.data);
	}

}
static void * rx_daemon(void * param)
{
	(void)param;
	struct Can_SocketHandle_s* handle;
	while(FALSE == socketH->terminated)
	{
		STAILQ_FOREACH(handle,&socketH->head,entry)
		{
			rx_notifiy(handle);
		}
	}

	return NULL;
}

/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* __WINDOWS__ */

