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
	struct sockaddr_in service;
	STAILQ_HEAD(,Can_SocketHandle_s) head;
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static struct Can_SocketHandleList_s* socketH = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
static int init_socket(int port)
{
	int ercd;
	WSADATA wsaData;
	struct timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 1600;

	socketH = malloc(sizeof(struct Can_SocketHandleList_s));
	assert(socketH);
	STAILQ_INIT(&socketH->head);

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	socketH->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketH->s == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		return FALSE;;
	}

	socketH->service.sin_family = AF_INET;
	socketH->service.sin_addr.s_addr = inet_addr("127.0.0.1");
	socketH->service.sin_port = (u_short)htons(CAN_PORT_MIN+port);
	ercd = bind(socketH->s, (SOCKADDR *) &(socketH->service), sizeof (SOCKADDR));
	if (ercd == SOCKET_ERROR) {
		wprintf(L"bind to port %d failed with error: %ld\n", port, WSAGetLastError());
		closesocket(socketH->s);
		return FALSE;
	}

	if (listen(socketH->s, CAN_BUS_NODE_MAX) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(socketH->s);
		return FALSE;
	}

   /* Set Timeout for recv call */
	if(setsockopt(socketH->s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == SOCKET_ERROR)
	{
		wprintf(L"setsockopt failed with error: %ld\n", WSAGetLastError());
		closesocket(socketH->s);
		return FALSE;
	}

	printf("can(%d) socket driver on-line!\n",port);
	return TRUE;
}
static void try_accept(void)
{
	struct Can_SocketHandle_s* handle;
	int s = accept(socketH->s, NULL, NULL);

	if(s != INVALID_SOCKET)
	{
		handle = malloc(sizeof(struct Can_SocketHandle_s));
		handle->s = s;
		STAILQ_INSERT_TAIL(&socketH->head,handle,entry);
		printf("can socket %X on-line!\n",s);
	}
	else
	{
		printf("accept failed\n");
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
static void try_recv_forward(void)
{
	int len;
	struct can_frame frame;
	struct Can_SocketHandle_s* h;
	struct Can_SocketHandle_s* h2;
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
						wprintf(L"send failed with error: %ld, remove this node!\n", WSAGetLastError());
						STAILQ_REMOVE(&socketH->head,h2,Can_SocketHandle_s,entry);
						closesocket(h2->s);
						free(h2);
					}
				}
			}
		}
		else
		{
			printf("timeout recv...\n");
		}
	}
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
