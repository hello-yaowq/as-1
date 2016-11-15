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
#ifdef __WINDOWS__
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#else
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/socket.h>
#ifdef linux
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#endif /* linux */
#endif
#include <asdebug.h>
/* ============================ [ MACROS    ] ====================================================== */
#ifdef linux
#define DEVTAP "/dev/net/tun"
#else  /* linux */
#define DEVTAP "/dev/tap0"
#endif /* linux */

#define DROP 0

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#define AS_LOG_TAP 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
#ifdef USE_AS_ETH
static int client_sockfd = -1;
#else
#if DROP
static int drop = 0;
#endif

static int fd;

static unsigned long lasttime;

#endif
/* ============================ [ LOCALS    ] ====================================================== */
#ifdef USE_AS_ETH
#else
static void
remove_route(void)
{
  char buf[1024];
  snprintf(buf, sizeof(buf), "route delete -net 172.18.0.0");
  system(buf);
  fprintf(stderr, "%s\n", buf);

}
static void log_msg(unsigned char* frame,size_t len,bool read)
{
	size_t i,j;
	printf("\n%8s :: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  length=%d\n",read?"read":"write",(int)len);
	for(i=0; i<(len+15)/16; i++)
	{

		printf("%08X ::",(uint32_t)i*16);

		fflush(stdout);
		for(j=0;j<16;j++)
		{
			if((i*16+j)<len)
			{
				printf(" %02X",frame[i*16+j]);
			}
			else
			{
				printf("   ");
			}
		}
		printf("\t");
		for(j=0;j<16;j++)
		{
			if(((i*16+j)<len) && isprint(frame[i*16+j]))
			{
				printf("%c",frame[i*16+j]);
			}
			else
			{
				printf(".");
			}
		}
		printf("\n");
	}

	fflush(stdout);
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
#ifdef USE_AS_ETH
void lwip_client_init(void)
{
	#ifdef __WINDOWS__
	WSADATA wsaData;
	#endif
	struct sockaddr_in remote_addr;
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET;
	remote_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	remote_addr.sin_port=htons(8000);

	#ifdef __WINDOWS__
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	#endif

	if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("lwip:socket");
		#ifdef __WINDOWS__
		WSACleanup();
		#endif
		asAssert(0);
	}

	if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
	{
		perror("lwip:connect");
		#ifdef __WINDOWS__
		closesocket(client_sockfd);
		#endif
		asAssert(0);
	}
}

int lwip_client_send(void* data,size_t size)
{
	int len;

	asCallStack();
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
	len=recv(client_sockfd,data,size,MSG_DONTWAIT);

	if( len < 0)
	{
		if(-1 != len) { printf("lwip: client recv error %d\n",len); }
		len = 0;	/* ETH_ERROR */
	}

	return len;
}
#else
int
tapdev_fd(void)
{
  return fd;
}
void lwip_client_init(void)
{
  char buf[1024];

  fd = open(DEVTAP, O_RDWR);
  if(fd == -1) {
    perror("tapdev: tapdev_init: open");
    return;
  }

#ifdef linux
  {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
    if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
      perror("ioctl(TUNSETIFF)");
      exit(1);
    }
  }
#endif /* Linux */

  snprintf(buf, sizeof(buf), "ifconfig tap0 inet 172.18.0.1/16");
  system(buf);
  fprintf(stderr, "%s\n", buf);
#ifdef linux
  /* route add for linux */
  snprintf(buf, sizeof(buf), "route add -net 172.18.0.0/16 dev tap0");
#else /* linux */
  /* route add for freebsd */
  snprintf(buf, sizeof(buf), "route add -net 172.18.0.0/16 -iface tap0");
#endif /* linux */

  system(buf);
  fprintf(stderr, "%s\n", buf);
  atexit(remove_route);

  lasttime = 0;
}
/*---------------------------------------------------------------------------*/
int lwip_client_recv(void* data,size_t size)
{
  fd_set fdset;
  struct timeval tv;
  int ret;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&fdset);
  if(fd > 0) {
    FD_SET(fd, &fdset);
  }

  ret = select(fd + 1, &fdset, NULL, NULL, &tv);

  if(ret == 0) {
    return 0;
  }
  ret = read(fd, data, size);

  if(ret == -1) {
    //perror("tapdev_poll: read");
    ret = 0;
  }
  else
  {
	  log_msg(data,ret,1);
  }
  return ret;
}
/*---------------------------------------------------------------------------*/
int lwip_client_send(void* data,size_t size)
{
  int ret;

  if(fd <= 0) {
    return -2;
  }

  /*  printf("tapdev_send: sending %d bytes\n", size);*/
  /*  check_checksum(uip_buf, size);*/

#if DROP
  drop++;
  if(drop % 8 == 7) {
    fprintf(stderr, "Dropped an output packet!\n");
    return;
  }
#endif /* DROP */

  ret = write(fd, data, size);

  log_msg(data,size,0);

  if(ret == -1) {
    perror("tap_dev: tapdev_send: writev");
    exit(1);
  }

  return ret;
}
#endif
