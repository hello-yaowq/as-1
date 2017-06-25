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
#include <sys/types.h>
#include "lasdevlib.h"
#include "afb-wsj1.h"
#include "asocket.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define PPARAM(p) ((LAS_WebsockParamType*)p)
#define AS_LOG_LAS_DEV 0
/* ============================ [ TYPES     ] ====================================================== */
typedef struct {
	char* uri;
	int port;
	int s;
	struct afb_wsj1 * wsj1;
} LAS_WebsockParamType;
/* ============================ [ DECLARES  ] ====================================================== */
static int lasdev_open  (const char* device, lua_State *L, void** param);
static int lasdev_read  (void* param,lua_State *L);
static int lasdev_write (void* param,const char* data,size_t size);
static void lasdev_close(void* param);
static int lasdev_ioctl (void* param,int type, const char* data,size_t size);
static void on_close (void *, uint16_t code, char *, size_t size);
static void on_text (void *, char *, size_t size);
static void on_binary (void *, char *, size_t size);
static void on_error (void *, uint16_t code, const void *, size_t size);
static void on_hangup (void *);
/* ============================ [ DATAS     ] ====================================================== */
const LAS_DeviceOpsType websock_dev_ops = {
	.name = "websock",
	.open = lasdev_open,
	.read = lasdev_read,
	.write = lasdev_write,
	.close = lasdev_close,
	.ioctl = lasdev_ioctl
};
static const struct afb_wsj1_itf wsj1_itf =
{
	.on_hangup = on_hangup,
	.on_call = on_call,
	.on_event = on_event
};
/* ============================ [ LOCALS    ] ====================================================== */
static int lasdev_open  (const char* device, lua_State *L, void** param)
{
	char* uri;
	int s,port,is_server,is_num,n;
	size_t ls;
	n = lua_gettop(L);
	if(4==n)
	{
		uri = lua_tolstring(L, 2, &ls);
		if(0 == ls)
		{
			 return luaL_error(L,"incorrect argument uri name to function '%s'",__func__);
		}

		port = lua_tounsignedx(L, 3,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument port to function '%s'",__func__);
		}

		is_server = lua_tounsignedx(L, 4,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument port to function '%s'",__func__);
		}

		s = ask_create(is_server,uri,port);
		if(s < 0)
		{
			return luaL_error(L,"create websock %s on %s:%d failed at function '%s'",
					is_server?"server":"client",uri,port,__func__);
		}

		*param = malloc(sizeof(LAS_WebsockParamType));

		PPARAM(*param)->s = s;
		PPARAM(*param)->port = port;
		PPARAM(*param)->uri = strdup(uri);
		PPARAM(*param)->wsj1 = afb_wsj1_create(s,0,0);
	}
	else
	{
		return luaL_error(L, "%s (%s, baudrate,\"modes\") API should has 3 arguments",__func__);
	}
	return 0;
}
static int lasdev_read  (void* param,lua_State *L)
{
	return 0;
}
static int lasdev_write (void* param,const char* data,size_t size)
{
	return 0;
}
static void lasdev_close(void* param)
{
	return 0;
}
static int lasdev_ioctl (void* param,int type, const char* data,size_t size)
{
	return 0;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
