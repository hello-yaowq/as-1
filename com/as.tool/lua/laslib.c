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
/* LUA version is 5.3.1 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

#include <pthread.h>
#include <unistd.h>

#if defined(USE_RPMSG)
#include "RPmsg.h"
#endif

#include <sys/queue.h>
/* ============================ [ MACROS    ] ====================================================== */
#define CAN_RPMSG_BUS_NUM   4
/* ============================ [ TYPES     ] ====================================================== */
struct Can_RPmsgPud_s {
	Can_RPmsgPduType msg;
	STAILQ_ENTRY(Can_RPmsgPud_s) pduEntry;
};

struct Can_RPmsgPduQueue_s {
	int initialized;
	pthread_mutex_t w_lock;
	STAILQ_HEAD(,Can_RPmsgPud_s) pduHead;
};
/* ============================ [ DECLARES  ] ====================================================== */
static int luai_can_write (lua_State *L);
static int luai_can_read  (lua_State *L);
/* ============================ [ DATAS     ] ====================================================== */
static const luaL_Reg aslib[] = {
		{"can_write",luai_can_write},
		{"can_read", luai_can_read},
		{NULL,NULL}
};

static struct Can_RPmsgPduQueue_s canQ[CAN_RPMSG_BUS_NUM] =
{
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER}
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
				Std_ReturnType ercd;
				pduInfo.bus = busid;
				pduInfo.id = canid;
				pduInfo.length = dlc;
				memcpy(pduInfo.sdu,data,dlc);
				ASLOG(LUA,"LUA TX CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
					  pduInfo.id,pduInfo.length,pduInfo.sdu[0],pduInfo.sdu[1],pduInfo.sdu[2],pduInfo.sdu[3],
					  pduInfo.sdu[4],pduInfo.sdu[5],pduInfo.sdu[6],pduInfo.sdu[7]);

				do {
					ercd = RPmsg_Send(RPMSG_CHL_CAN,&pduInfo,sizeof(pduInfo));
				} while(ercd != E_OK);
			}
			#endif
		}

		lua_pushboolean(L, TRUE);        /* result OK */
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
		if( busid >= CAN_RPMSG_BUS_NUM)
		{
			return luaL_error(L,"busid out of range(%d > %d) to function 'can_read'",busid,CAN_RPMSG_BUS_NUM);
		}

		if(STAILQ_EMPTY(&canQ[busid].pduHead))
		{
			lua_pushboolean(L, FALSE);
			lua_pushnil(L);
			lua_pushnil(L);
		}
		else
		{
			struct Can_RPmsgPud_s* pdu;
			int table_index,i;

			lua_pushboolean(L, TRUE);

			(void)pthread_mutex_lock(&canQ[busid].w_lock);
			pdu = STAILQ_FIRST(&canQ[busid].pduHead);

			lua_pushinteger(L,pdu->msg.id);

			lua_newtable(L);
			table_index = lua_gettop(L);


			for(i=0; i<pdu->msg.length;i++)
			{
				lua_pushinteger(L, pdu->msg.sdu[i]);
				lua_seti(L, table_index, i+1);
			}
			STAILQ_REMOVE_HEAD(&canQ[busid].pduHead,pduEntry);
			(void)pthread_mutex_unlock(&canQ[busid].w_lock);

			free(pdu);
		}
		return 3;
	}
	else
	{
		return luaL_error(L, "can_read (bus_id) API should has 1 arguments");
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */
LUAMOD_API int (luaopen_as) (lua_State *L)
{
	int i;
	struct Can_RPmsgPud_s* pdu;

	for(i=0;i<CAN_RPMSG_BUS_NUM;i++)
	{
		(void)pthread_mutex_lock(&canQ[i].w_lock);
		if(canQ[i].initialized)
		{	/* free previous receive message */
			STAILQ_FOREACH(pdu,&canQ[i].pduHead,pduEntry ) {
				free(pdu);
			}
		}

		STAILQ_INIT(&canQ[i].pduHead);
		canQ[i].initialized = TRUE;
		(void)pthread_mutex_unlock(&canQ[i].w_lock);
	}
	luaL_newlib(L, aslib);
	return 1;
}

void luaclose_as(void)
{
	int i;
	struct Can_RPmsgPud_s* pdu;

	for(i=0;i<CAN_RPMSG_BUS_NUM;i++)
	{
		(void)pthread_mutex_lock(&canQ[i].w_lock);
		if(canQ[i].initialized)
		{	/* free previous receive message */
			STAILQ_FOREACH(pdu,&canQ[i].pduHead,pduEntry ) {
				free(pdu);
			}
		}

		STAILQ_INIT(&canQ[i].pduHead);
		canQ[i].initialized = FALSE;
		(void)pthread_mutex_unlock(&canQ[i].w_lock);
	}
}

#if defined(USE_RPMSG)
void Can_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	struct Can_RPmsgPud_s* pdu;
	Can_RPmsgPduType* pduInfo = (Can_RPmsgPduType*)data;
	asAssert(len==sizeof(Can_RPmsgPduType));
	asAssert(chl == RPMSG_CHL_CAN);

	if(pduInfo->bus < CAN_RPMSG_BUS_NUM)
	{
		if(canQ[pduInfo->bus].initialized)
		{

			pdu = malloc(sizeof(struct Can_RPmsgPud_s));
			if(pdu)
			{
				memcpy(&(pdu->msg),pduInfo,sizeof(Can_RPmsgPduType));
				(void)pthread_mutex_lock(&canQ[pduInfo->bus].w_lock);
				STAILQ_INSERT_TAIL(&canQ[pduInfo->bus].pduHead,pdu,pduEntry);
				(void)pthread_mutex_unlock(&canQ[pduInfo->bus].w_lock);
			}
			else
			{
				ASWARNING("LUA CAN RX malloc failed\n");
			}
		}
		else
		{
			/* not on-line */
			ASLOG(LUA,"lua is not on-line now!\n");
		}
	}
	else
	{
		ASWARNING("LUA CAN RX bus <%d> out of range, busid < %d is support only\n",pduInfo->bus,CAN_RPMSG_BUS_NUM);
	}

    ASLOG(LUA,"RPMAG RX CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
		  pduInfo->id,pduInfo->length,pduInfo->sdu[0],pduInfo->sdu[1],pduInfo->sdu[2],pduInfo->sdu[3],
		  pduInfo->sdu[4],pduInfo->sdu[5],pduInfo->sdu[6],pduInfo->sdu[7]);
}
void Can_RPmsg_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_CAN);
}
#endif
