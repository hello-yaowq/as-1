#ifdef __SOCKET_WIN_CAN_DRIVER__
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
#include <sys/queue.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>

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
#define CAN_BUS_NODE_MAX 32	/* maximum node on the bus port */
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
	int s; /* can raw socket: accept */
	STAILQ_ENTRY(Can_SocketHandle_s) entry;
};
struct Can_SocketHandleList_s
{
	int s; /* can raw socket: listen */
	pthread_t rx_thread;
	STAILQ_HEAD(,Can_SocketHandle_s) head;
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static struct Can_SocketHandleList_s* socketH = NULL;
static pthread_mutex_t socketLock = PTHREAD_MUTEX_INITIALIZER;
/* ============================ [ LOCALS    ] ====================================================== */
static int init_socket(int port)
{
	int ercd;
	int s;
	WSADATA wsaData;
	struct sockaddr_in service;
	struct timeval tv;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		return FALSE;;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = (u_short)htons(CAN_PORT_MIN+port);
	ercd = bind(s, (SOCKADDR *) &(service), sizeof (SOCKADDR));
	if (ercd == SOCKET_ERROR) {
		wprintf(L"bind to port %d failed with error: %ld\n", port, WSAGetLastError());
		closesocket(s);
		return FALSE;
	}

	if (listen(s, CAN_BUS_NODE_MAX) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(s);
		return FALSE;
	}

    /* Set Timeout for recv call */
	tv.tv_sec  = 0;
	tv.tv_usec = 0;
	if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == SOCKET_ERROR)
	{
		wprintf(L"setsockopt failed with error: %ld\n", WSAGetLastError());
		closesocket(s);
		return FALSE;
	}

	printf("can(%d) socket driver on-line!\n",port);

	socketH = malloc(sizeof(struct Can_SocketHandleList_s));
	assert(socketH);
	STAILQ_INIT(&socketH->head);
	socketH->s = s;

	return TRUE;
}
static void try_accept(void)
{
	struct Can_SocketHandle_s* handle;
	struct timeval tv;
	int s = accept(socketH->s, NULL, NULL);

	if(s != INVALID_SOCKET)
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
		handle = malloc(sizeof(struct Can_SocketHandle_s));
		handle->s = s;
	    /* Set Timeout for recv call */
		if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == SOCKET_ERROR)
		{
			wprintf(L"setsockopt failed with error: %ld\n", WSAGetLastError());
			closesocket(s);
			return;
		}
		pthread_mutex_lock(&socketLock);
		STAILQ_INSERT_TAIL(&socketH->head,handle,entry);
		pthread_mutex_unlock(&socketLock);
		printf("can socket %X on-line!\n",s);
	}
	else
	{
		wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
	}
}
static void * rx_daemon(void * param)
{
	(void)param;
	while(TRUE)
	{
		try_accept();
	}

	return NULL;
}
static void remove_socket(struct Can_SocketHandle_s* h)
{
	STAILQ_REMOVE(&socketH->head,h,Can_SocketHandle_s,entry);
	closesocket(h->s);
	free(h);
}
static void try_recv_forward(void)
{
	int len;
	struct can_frame frame;
	struct Can_SocketHandle_s* h;
	struct Can_SocketHandle_s* h2;
	pthread_mutex_lock(&socketLock);
	STAILQ_FOREACH(h,&socketH->head,entry)
	{
		len = recv(h->s, (void*)&frame, CAN_MTU, 0);
		if(CAN_MTU == len)
		{
			printf("canid=%08X,dlc=%d,data=[%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X]\n",
					frame.can_id,frame.can_dlc,
					frame.data[0],frame.data[1],frame.data[2],frame.data[3],
					frame.data[4],frame.data[5],frame.data[6],frame.data[7]);
			STAILQ_FOREACH(h2,&socketH->head,entry)
			{
				if(h != h2)
				{
					if (send(h2->s, (const char*)&frame, CAN_MTU,0) != CAN_MTU) {
						wprintf(L"send failed with error: %ld, remove this node %X!\n", WSAGetLastError(),h2->s);
						remove_socket(h2);
						break;
					}
				}
			}
		}
		else if(-1 == len)
		{
			wprintf(L"recv failed with error: %ld, remove this node %X!\n", WSAGetLastError(),h->s);
			remove_socket(h);
			break;
		}
		else
		{
			printf("timeout recv... len=%d\n",len);
		}
	}
	pthread_mutex_unlock(&socketLock);
}
static void schedule(void)
{
	try_recv_forward();
}
/* ============================ [ FUNCTIONS ] ====================================================== */
int main(int argc,char* argv[])
{
	int rv;
	if(argc != 2)
	{
		printf("Usage:%s <port>\n",argv[0]);
		return -1;
	}

	if(FALSE==init_socket(atoi(argv[1])))
	{
		WSACleanup();
		return -1;
	}

	if( 0 == pthread_create(&(socketH->rx_thread),NULL,rx_daemon,NULL))
	{
	}
	else
	{
		return -1;
	}

	for(;;)
	{
		schedule();
	}

	return 0;
}
#endif
