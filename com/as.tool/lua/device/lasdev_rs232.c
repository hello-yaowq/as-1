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
#include "lasdevlib.h"
#include "rs232.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define PPARAM(p) ((LAS_RS232ParamType*)p)
/* ============================ [ TYPES     ] ====================================================== */
enum
{
	RW_STRING_MODE,
	RW_RAW_MODE
};
typedef struct {
	int port;
	int mode;
} LAS_RS232ParamType;
/* ============================ [ DECLARES  ] ====================================================== */
static int lasdev_open  (const char* device, lua_State *L, void** param);
static int lasdev_read  (void* param,lua_State *L);
static int lasdev_write (void* param);
static int lasdev_close (void* param);
static void lasdev_ioctl(void* param);
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
	const LAS_DeviceOpsType* ops;
	size_t ls;
	int is_num,n,baudrate,port;

	port = atoi(&device[3])-1;

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

		modes = lua_tolstring(L, 3, &ls);
		if(0 == ls)
		{
			 return luaL_error(L,"incorrect argument mode to function '%s','8N1' is an example",__func__);
		}
		ASLOG(ON,"rs232 open(%d,%d,%s)\n",port,baudrate,modes);

		PPARAM(*param)->mode = RW_STRING_MODE;
		if(4u == strlen(modes))
		{
			if('R' ==modes[3])
			{
				PPARAM(*param)->mode = RW_RAW_MODE;
			}
			modes[3] = 0;
		}

		if(0 == RS232_OpenComport(port,baudrate,modes))
		{
			return 1;
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

	len = RS232_PollComport(PPARAM(param)->port,data,sizeof(data)-1);

	ASLOG(ON,"rs232 %d = read(%d)\n",len,*((LAS_RS232ParamType*)param));

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
static int lasdev_write (void* param)
{

	return 0;
}
static int lasdev_close (void* param)
{
	RS232_CloseComport(PPARAM(param)->port);
	free(param);
	return 0;
}
static void lasdev_ioctl(void* param)
{

}
/* ============================ [ FUNCTIONS ] ====================================================== */
