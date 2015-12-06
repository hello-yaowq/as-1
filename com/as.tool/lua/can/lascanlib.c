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
#include "Std_Types.h"
#include "lascanlib.h"
#include <sys/queue.h>
#include <pthread.h>
#include <unistd.h>
/* ============================ [ MACROS    ] ====================================================== */
#define CAN_BUS_NUM   4
#define CAN_BUS_PDU_NUM   16
/* ============================ [ TYPES     ] ====================================================== */
typedef struct {
    /* the CAN ID, 29 or 11-bit */
    uint32_t 	id;
    uint8_t     bus;
    /* Length, max 8 bytes */
    uint8_t		length;
    /* data ptr */
    uint8_t 		sdu[8];
} Can_PduType;
struct Can_Pdu_s {
	Can_PduType msg;
	STAILQ_ENTRY(Can_Pdu_s) pduEntry;
};
struct Can_PduList_s {
	uint32_t id;	/* can_id of this list */
	uint32_t size;
	STAILQ_HEAD(,Can_Pdu_s) pduHead;
	STAILQ_ENTRY(Can_PduList_s) listEntry;
};
struct Can_PduQueue_s {
	int initialized;
	pthread_mutex_t w_lock;
	Can_DeviceType  device;
	STAILQ_HEAD(,Can_PduList_s) listHead;
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static struct Can_PduQueue_s canQ[CAN_BUS_NUM] =
{
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER},
	{.initialized=FALSE,.w_lock=PTHREAD_MUTEX_INITIALIZER}
};
static const Can_DeviceOpsType* canOps [] =
{
	&can_rpmsg_ops,
	NULL
};
/* ============================ [ LOCALS    ] ====================================================== */
static void freeL(struct Can_PduList_s* l)
{
	struct Can_Pdu_s* pdu;
	while(FALSE == STAILQ_EMPTY(&l->pduHead))
	{
		pdu = STAILQ_FIRST(&l->pduHead);
		STAILQ_REMOVE_HEAD(&l->pduHead,pduEntry);

		free(pdu);
	}
}
static void freeQ(struct Can_PduQueue_s* q)
{
	struct Can_PduList_s* l;
	while(FALSE == STAILQ_EMPTY(&q->listHead))
	{
		l = STAILQ_FIRST(&q->listHead);
		freeL(l);
		STAILQ_REMOVE_HEAD(&q->listHead,listEntry);

		free(l);
	}
}
static void saveQ(struct Can_PduQueue_s* q,struct Can_Pdu_s* pdu)
{
	struct Can_PduList_s* L;
	struct Can_PduList_s* l;
	L = NULL;
	STAILQ_FOREACH(l,&q->listHead,listEntry)
	{
		if(l->id == pdu->msg.id)
		{
			L = l;
			break;
		}
	}

	if(NULL == L)
	{
		L = malloc(sizeof(struct Can_PduList_s));
		if(L)
		{
			L->id = pdu->msg.id;
			L->size = 0;
			STAILQ_INIT(&L->pduHead);
			STAILQ_INSERT_TAIL(&q->listHead,L,listEntry);
		}
		else
		{
			ASWARNING("LUA CAN Q List malloc failed\n");
		}
	}

	if(L)
	{
		if(L->size < CAN_BUS_PDU_NUM)
		{
			STAILQ_INSERT_TAIL(&L->pduHead,pdu,pduEntry);
			L->size ++;
		}
		else
		{
			ASWARNING("LUA CAN Q[id=%X] List is full with size %d\n",L->id,L->size);
			free(pdu);
		}
	}
}
static void rx_notification(uint32_t busid,uint32_t canid,uint32_t dlc,uint8_t* data)
{
	if(busid < CAN_BUS_NUM)
	{
		if(canQ[busid].initialized)
		{
			struct Can_Pdu_s* pdu = malloc(sizeof(struct Can_Pdu_s));
			if(pdu)
			{
				pdu->msg.bus = busid;
				pdu->msg.id = canid;
				pdu->msg.length = dlc;
				memcpy(&(pdu->msg.sdu),data,dlc);
				(void)pthread_mutex_lock(&canQ[busid].w_lock);
				saveQ(&canQ[busid],pdu);
				(void)pthread_mutex_unlock(&canQ[busid].w_lock);
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
		ASWARNING("LUA CAN RX bus <%d> out of range, busid < %d is support only\n",busid,CAN_BUS_NUM);
	}

    ASLOG(LUA,"RPMAG RX CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
		  canid,dlc,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
}
static const Can_DeviceOpsType* search_ops(const char* name)
{
	const Can_DeviceOpsType *ops,**o;
	o = canOps;
	ops = NULL;
	while(*o != NULL)
	{
		if(0 == strcmp((*o)->name,name))
		{
			ops = *o;
			break;
		}
		o++;
	}

	return ops;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
int luai_can_open  (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(4==n)
	{
		uint32_t busid,port,baudrate;
		const char* device_name;
		const Can_DeviceOpsType* ops;
		size_t ls;
		int is_num;

		busid = lua_tounsignedx(L, 1,&is_num);
		if((!is_num) || (busid >= CAN_BUS_NUM))
		{
			 return luaL_error(L,"incorrect argument busid to function 'can_open'");
		}

		device_name = lua_tolstring(L, 2, &ls);
		if((0 == ls) || (ls > CAN_DEVICE_NAME_SIZE))
		{
			 return luaL_error(L,"incorrect argument device name to function 'can_open'");
		}

		port = lua_tounsignedx(L, 3, &is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument port to function 'can_open'");
		}


		baudrate = lua_tounsignedx(L, 4, &is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument baudrate to function 'can_open'");
		}

		if(canQ[busid].initialized)
		{
			return luaL_error(L,"can bus(%d) is already on-line 'can_open'",busid);
		}
		else
		{
			ops = search_ops(device_name);
			if(NULL != ops)
			{
				canQ[busid].initialized = TRUE;
				canQ[busid].device.ops = ops;
				canQ[busid].device.busid = busid;
				canQ[busid].device.port = port;

				boolean rv = ops->probe(busid,port,baudrate,rx_notification);

				if(rv)
				{
					lua_pushboolean(L, TRUE);        /* result OK */
				}
				else
				{
					return luaL_error(L, "can_open device <%s> failed!",device_name);
				}
			}
			else
			{
				return luaL_error(L, "can_open device <%s> is not known by lua!",device_name);
			}
		}


		return 1;
	}
	else
	{
		return luaL_error(L, "can_open (bus_id,\"device name\",port,baudrate) API should has 3 arguments");
	}
}
int luai_can_write (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(3==n)
	{
		uint32 busid,canid,dlc;
		uint8 data[8];
		int is_num;

		busid = lua_tounsignedx(L, 1,&is_num);
		if((!is_num) || (busid >= CAN_BUS_NUM))
		{
			 return luaL_error(L,"incorrect argument busid to function 'can_write'");
		}

		canid = lua_tounsignedx(L, 2,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument canid to function 'can_write'");
		}

		dlc = luaL_len ( L , 3 ) ;
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
		}

		if(FALSE == canQ[busid].initialized)
		{
			return luaL_error(L,"can bus(%d) is not on-line 'can_write'",busid);
		}
		else
		{
			if(canQ[busid].device.ops->write)
			{
				boolean rv = canQ[busid].device.ops->write(canQ[busid].device.port,canid,dlc,data);
				if(rv)
				{
					lua_pushboolean(L, TRUE);        /* result OK */
				}
				else
				{
					return luaL_error(L, "can_write bus(%d) failed!",busid);
				}
			}
			else
			{
				return luaL_error(L,"can bus(%d) is read-only 'can_write'",busid);
			}
		}

		return 1;
	}
	else
	{
		return luaL_error(L, "can_write (bus_id,can_id,{xx,xx,xx,xx,xx,xx,xx,xx}) API should has 3 arguments");
	}
}

int luai_can_read  (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(2==n)
	{
		uint32_t busid;
		uint32_t canid;
		struct Can_PduList_s* l;
		struct Can_PduList_s* list = NULL;
		struct Can_Pdu_s* pdu = NULL;
		int is_num;

		busid = lua_tounsignedx(L, 1,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument busid to function 'can_read'");
		}
		if( busid >= CAN_BUS_NUM)
		{
			return luaL_error(L,"busid out of range(%d > %d) to function 'can_read'",busid,CAN_BUS_NUM);
		}

		canid = lua_tounsignedx(L, 2,&is_num);
		if(!is_num)
		{
			 return luaL_error(L,"incorrect argument canid to function 'can_read'");
		}

		(void)pthread_mutex_lock(&canQ[busid].w_lock);
		STAILQ_FOREACH(l,&canQ[busid].listHead,listEntry)
		{
			if(l->id == canid)
			{
				list = l;
				break;
			}
		}
		if(list && (FALSE == STAILQ_EMPTY(&list->pduHead)))
		{
			pdu = STAILQ_FIRST(&list->pduHead);
			STAILQ_REMOVE_HEAD(&list->pduHead,pduEntry);
			list->size --;
		}
		(void)pthread_mutex_unlock(&canQ[busid].w_lock);

		if(NULL == pdu)
		{
			lua_pushboolean(L, FALSE);
			lua_pushnil(L);
			lua_pushnil(L);
		}
		else
		{
			int table_index,i;

			lua_pushboolean(L, TRUE);
			lua_pushinteger(L,pdu->msg.id);
			lua_newtable(L);
			table_index = lua_gettop(L);
			for(i=0; i<pdu->msg.length;i++)
			{
				lua_pushinteger(L, pdu->msg.sdu[i]);
				lua_seti(L, table_index, i+1);
			}
			free(pdu);
		}
		return 3;
	}
	else
	{
		return luaL_error(L, "can_read (bus_id, can_id) API should has 2 arguments");
	}
}

void luai_canlib_open(void)
{
	int i;

	for(i=0;i<CAN_BUS_NUM;i++)
	{
		(void)pthread_mutex_lock(&canQ[i].w_lock);
		if(canQ[i].initialized)
		{	/* free previous receive message */
			freeQ(&canQ[i]);
		}

		STAILQ_INIT(&canQ[i].listHead);
		canQ[i].initialized = FALSE;
		(void)pthread_mutex_unlock(&canQ[i].w_lock);
	}
}
void luai_canlib_close(void)
{
	int i;

	for(i=0;i<CAN_BUS_NUM;i++)
	{
		(void)pthread_mutex_lock(&canQ[i].w_lock);
		if(canQ[i].initialized)
		{	/* free previous receive message */
			freeQ(&canQ[i]);
		}

		STAILQ_INIT(&canQ[i].listHead);
		canQ[i].initialized = FALSE;
		(void)pthread_mutex_unlock(&canQ[i].w_lock);
	}
}


