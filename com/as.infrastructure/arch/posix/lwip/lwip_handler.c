/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2016  AS <parai@foxmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asdebug.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static int client_sockfd = -1;
/* ============================ [ LOCALS    ] ====================================================== */

/* ============================ [ FUNCTIONS ] ====================================================== */
void lwip_client_init(void)
{
	struct sockaddr_in remote_addr;
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET;
	remote_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	remote_addr.sin_port=htons(8000);

	if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("lwip:socket");
		asAssert(0);
	}

	if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
	{
		perror("lwip:connect");
		asAssert(0);
	}
}

int lwip_client_send(void* data,size_t size)
{
	int len;

	len=send(client_sockfd,data,size,0);

	if( len < 0)
	{
		printf("lwip: client send error %d\n",len);
		len = 0;	/* ETH_ERROR */
	}
	return len;
}

int lwip_client_recv(void* data,size_t size)
{
	int len;
	len=recv(client_sockfd,data,size,0);

	if( len < 0)
	{
		if(-1 != len) { printf("lwip: client recv error %d\n",len); }
		len = 0;	/* ETH_ERROR */
	}

	return len;
}
