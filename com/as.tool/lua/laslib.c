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
#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

#if defined(USE_RPMSG)
#include "RPmsg.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct {
    /* the CAN ID, 29 or 11-bit */
    uint32_t 	id;
    uint8_t     bus;
    /* Length, max 8 bytes */
    uint8_t		length;
    /* data ptr */
    uint8_t 		sdu[8];
} Can_RPmsgPduType;
/* ============================ [ DECLARES  ] ====================================================== */
static int luai_can_write (lua_State *L);
static int luai_can_read  (lua_State *L);
/* ============================ [ DATAS     ] ====================================================== */
static const luaL_Reg aslib[] = {
		{"can_write",luai_can_write},
		{"can_read", luai_can_read},
		{NULL,NULL}
};
/* ============================ [ LOCALS    ] ====================================================== */
static int luai_can_write (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(3==n)
	{
		uint32 busid,canid;
		uint8 data[8];
		int is_num;

		busid = lua_tounsignedx(L, 1,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument busid to function 'can_write'");
		}

		canid = lua_tounsignedx(L, 2,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument canid to function 'can_write'");
		}

		int dlc = luaL_len ( L , 3 ) ;
		if(dlc > 8)
		{
			return luaL_error(L,"len(data array{})>8 to function 'can_write'");
		}
		else
		{
			int i = 0;
			/* Push another reference to the table on top of the stack (so we know
			 * where it is, and this function can work for negative, positive and
			 * pseudo indices
			 */
			lua_pushvalue(L, 3);
			/* stack now contains: -1 => table */
			lua_pushnil(L);
			/* stack now contains: -1 => nil; -2 => table */
			while (lua_next(L, -2))
			{
				/* stack now contains: -1 => value; -2 => key; -3 => table
				 * copy the key so that lua_tostring does not modify the original */
				lua_pushvalue(L, -2);
				/* stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
				 * printf("%s => %s\n", lua_tostring(L, -1), lua_tostring(L, -2)); */
				data[i] = lua_tounsignedx(L, -2,&is_num);
				if(!is_num)
				{
					return luaL_error(L,"invalid data[%d] to function 'can_write'",i);
				}
				else
				{
					i ++;
				}
				/* pop value + copy of key, leaving original key */
				lua_pop(L, 2);
				/* stack now contains: -1 => key; -2 => table */
			}
			/* stack now contains: -1 => table (when lua_next returns 0 it pops the key
			 * but does not push anything.)
			 * Pop table */
			lua_pop(L, 1);
			/* Stack is now the same as it was on entry to this function */
			#if defined(USE_RPMSG)
			if(RPmsg_IsOnline())
			{
				Can_RPmsgPduType pduInfo;
				pduInfo.bus = busid;
				pduInfo.id = canid;
				pduInfo.length = dlc;
				memcpy(pduInfo.sdu,data,dlc);
				ASLOG(LUA,"LUA TX CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
					  pduInfo.id,pduInfo.length,pduInfo.sdu[0],pduInfo.sdu[1],pduInfo.sdu[2],pduInfo.sdu[3],
					  pduInfo.sdu[4],pduInfo.sdu[5],pduInfo.sdu[6],pduInfo.sdu[7]);

				RPmsg_Send(RPMSG_CHL_CAN,&pduInfo,sizeof(pduInfo));
			}
			#endif
		}

		lua_pushnumber(L, 0);        /* result OK */
		return 1;
	}
	else
	{
		return luaL_error(L, "can_write (bus_id,can_id,{xx,xx,xx,xx,xx,xx,xx,xx}) API should has 3 arguments");
	}
}

static int luai_can_read  (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(1==n)
	{
		uint32 busid;
		int is_num;

		busid = lua_tounsignedx(L, 1,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument busid to function 'can_read'");
		}

		lua_pushnumber(L, busid);        /* result OK */
		return 1;
	}
	else
	{
		return luaL_error(L, "can_read (bus_id) API should has 1 arguments");
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */

LUAMOD_API int (luaopen_as) (lua_State *L)
{
	luaL_newlib(L, aslib);
	return 1;
}

#if defined(USE_RPMSG)
void Can_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	Can_RPmsgPduType* pduInfo = (Can_RPmsgPduType*)data;
	asAssert(len==sizeof(Can_RPmsgPduType));
	asAssert(chl == RPMSG_CHL_CAN);

    ASLOG(CAN,"RPMAG RX CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
		  pduInfo->id,pduInfo->length,pduInfo->sdu[0],pduInfo->sdu[1],pduInfo->sdu[2],pduInfo->sdu[3],
		  pduInfo->sdu[4],pduInfo->sdu[5],pduInfo->sdu[6],pduInfo->sdu[7]);
}
void Can_RPmsg_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_CAN);
}
#endif
