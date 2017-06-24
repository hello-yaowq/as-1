/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017 AS <parai@foxmail.com>
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
#ifdef __WINDOWS__
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <sys/types.h>
#include "lasdevlib.h"
#include "rs232.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define PPARAM(p) ((LAS_RS232ParamType*)p)
#define AS_LOG_LAS_DEV 0
/* QEMU TCP 127.0.0.1:1103 't' = 0x74, 'c' = 0x63, 'p' = 0x70 */
#define CAN_TCP_SERIAL_PORT 0x746370
/* ============================ [ TYPES     ] ====================================================== */
enum
{
	RW_STRING_MODE,
	RW_RAW_MODE
};
typedef struct {
	int port;
	int mode;
	int s;
} LAS_RS232ParamType;
/* ============================ [ DECLARES  ] ====================================================== */
static int lasdev_open  (const char* device, lua_State *L, void** param);
static int lasdev_read  (void* param,lua_State *L);
static int lasdev_write (void* param,const char* data,size_t size);
static void lasdev_close(void* param);
static int lasdev_ioctl (void* param,int type, const char* data,size_t size);
/* ============================ [ DATAS     ] ====================================================== */
const LAS_DeviceOpsType rs232_dev_ops = {
	.name = "COM",
	.open = lasdev_open,
	.read = lasdev_read,
	.write = lasdev_write,
	.close = lasdev_close,
	.ioctl = lasdev_ioctl
};
/* ============================ [ LOCALS    ] ====================================================== */
static int lasdev_open  (const char* device, lua_State *L, void** param)
{
	char* modes;
	size_t ls;
	int is_num,n,baudrate,port;

	if(0 == strcmp(device,"COMTCP"))
	{
		port = CAN_TCP_SERIAL_PORT;
	}
	else
	{
		port = atoi(&device[3])-1;
	}

	*param = malloc(sizeof(LAS_RS232ParamType));

	PPARAM(*param)->port = port;

	n = lua_gettop(L);  /* number of arguments */
	if(3==n)
	{
		baudrate = lua_tounsignedx(L, 2,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument baudrate to function '%s'",__func__);
		}

		modes = (char*)lua_tolstring(L, 3, &ls);
		if(0 == ls)
		{
			 return luaL_error(L,"incorrect argument mode to function '%s','8N1' is an example",__func__);
		}
        ASLOG(LAS_DEV,"rs232 open(%d,%d,%s)\n",port,baudrate,modes);

		PPARAM(*param)->mode = RW_STRING_MODE;
		if(4u == strlen(modes))
		{
			if('R' ==modes[3])
			{
				PPARAM(*param)->mode = RW_RAW_MODE;
			}
			modes[3] = 0;
		}

		if(CAN_TCP_SERIAL_PORT == port)
		{
			int s,rv;
			struct sockaddr_in addr;
			#ifdef __WINDOES__
			{
				WSADATA wsaData;
				WSAStartup(MAKEWORD(2, 2), &wsaData);
			}
			#endif

			rv = TRUE;
			memset(&addr,0,sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			addr.sin_port = htons(1103);

			if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				ASWARNING("Serial TCP open failed!\n");
				rv = FALSE;
			}

			if( rv )
			{
				/* Connect to server. */
				if(connect(s, (struct sockaddr*) & addr, sizeof (addr)) < 0)
				{
					#ifdef __WINDOES__
					ASWARNING("Serial TCP connect failed: %d\n", WSAGetLastError());
					closesocket(s);
					#else
					ASWARNING("Serial TCP connect failed!\n");
					close(s);
					#endif
					rv = FALSE;
				}
			}

			if(rv)
			{
				PPARAM(*param)->s = s;
				return 1;
			}
		}
		else if(0 == RS232_OpenComport(port,baudrate,modes))
		{
			return 1;
		}
		else
		{
		}
	}
	else
	{
		return luaL_error(L, "%s (%s, baudrate,\"modes\") API should has 3 arguments",__func__);
	}

	return 0;
}
static int lasdev_read  (void* param,lua_State *L)
{
	char data[4097]; /* maximum read size 4096 */
	int len;
	int table_index,i;


	if(CAN_TCP_SERIAL_PORT == PPARAM(param)->port)
	{
		len = recv(PPARAM(param)->s,data,sizeof(data)-1,0);
	}
	else
	{
		len = RS232_PollComport(PPARAM(param)->port,(unsigned char*)data,sizeof(data)-1);
	}

	ASLOG(LAS_DEV,"rs232 %d = read(%d)\n",len,*((LAS_RS232ParamType*)param));

	lua_pushinteger(L,len);

	if(len > 0)
	{
		if(PPARAM(param)->mode == RW_STRING_MODE)
		{
			lua_pushlstring(L,data,len);
		}
		else
		{
			lua_newtable(L);
			table_index = lua_gettop(L);
			for(i=0; i<len;i++)
			{
				lua_pushinteger(L, data[i]);
				lua_seti(L, table_index, i+1);
			}
		}
	}
	else
	{
		lua_pushnil(L);
	}

	return 2;
}
static int lasdev_write (void* param,const char* data,size_t ls)
{
	int size = 0;

	if( CAN_TCP_SERIAL_PORT == PPARAM(param)->port)
	{
		if(ls == (size_t)send(PPARAM(param)->s, (const char*)data, ls,0))
		{
			/* send OK */
			size = ls;
		}
		else
		{
			ASWARNING("Serial TCP send message failed!\n");
		}
	}
	else
	{
		size = RS232_SendBuf(PPARAM(param)->port,(unsigned char*)data,ls);
	}

	return size;
}
static void lasdev_close (void* param)
{
	if( CAN_TCP_SERIAL_PORT == PPARAM(param)->port)
	{
		#ifdef __WINDOES__
		closesocket(PPARAM(param)->s);
		#else
		close(PPARAM(param)->s);
		#endif
	}
	else
	{
		RS232_CloseComport(PPARAM(param)->port);
	}
	free(param);
}
static int lasdev_ioctl(void* param,int type, const char* data,size_t size)
{
	(void) param;
	(void) type;
	(void) data;
	(void) size;
	return 0;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
