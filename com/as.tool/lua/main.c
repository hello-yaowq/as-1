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
#include <unistd.h>
#include "Std_Types.h"

#if defined(USE_IPC)
#include "Ipc.h"
#endif

#if defined(USE_VIRTQ)
#include "VirtQ.h"
#endif

#if defined(USE_RPMSG)
#include "RPmsg.h"
#endif

#include "shell.h"

/* ============================ [ MACROS    ] ====================================================== */
#ifndef USE_SHELL
#error "macro USE_SHELL is not defined"
#endif

#define SHELL_CMD_CACHE_SIZE  4096
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
typedef struct {
	uint32_t r_pos;
	uint32_t w_pos;
	volatile uint32_t counter;
	char     cmd[SHELL_CMD_CACHE_SIZE];
}Shel_CmdInputCacheType;
/* ============================ [ DECLARES  ] ====================================================== */
extern int lua_main(int argc, char *argv[]);
/* ============================ [ DATAS     ] ====================================================== */
static Shel_CmdInputCacheType shCmdCache;
static ShellCmdT luacmd =
{
	.func = lua_main,
	.argMin = 0,
	.argMax = 0,
	.cmd = "lua",
	.shortDesc = "lua <script>",
	.longDesc ="lua script executor",
};
/* ============================ [ LOCALS    ] ====================================================== */
static void StartupHook(void)
{
#if defined(USE_IPC)
	Ipc_Init(&Ipc_Config);
#endif

#if defined(USE_VIRTQ)
	VirtQ_Init(&VirtQ_Config);
#endif

#if defined(USE_RPMSG)
	RPmsg_Init(&RPmsg_Config);
#endif

	memset(&shCmdCache,0,sizeof(shCmdCache));
	SHELL_Init();
	SHELL_AddCmd(&luacmd);
}
/* ============================ [ FUNCTIONS ] ====================================================== */
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
void Shell_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	uint32_t i;
	char* cmd = (char*)data;
	asAssert(chl == RPMSG_CHL_SHELL);
	ASLOG(SHELL,"receive cmd \"%s\"\n",cmd);
	if( (shCmdCache.counter+len) < SHELL_CMD_CACHE_SIZE)
	{
		for(i=0;i<len;i++)
		{
			shCmdCache.cmd[shCmdCache.w_pos] = cmd[i];
			shCmdCache.w_pos ++;
			if(shCmdCache.w_pos >= SHELL_CMD_CACHE_SIZE)
			{
				shCmdCache.w_pos = 0;
			}
			shCmdCache.counter++;
		}
	}
	else
	{
		ASLOG(SHELL,"command buffer full, ignore cmd \"%s\"\n",cmd);
	}
}
char SHELL_getc(void)
{
	char chr;
	while(0 == shCmdCache.counter)
	{
		usleep(1);
	}
	chr = shCmdCache.cmd[shCmdCache.r_pos];
	shCmdCache.r_pos++;
	if(shCmdCache.r_pos >= SHELL_CMD_CACHE_SIZE)
	{
		shCmdCache.r_pos = 0;
	}
	shCmdCache.counter--;
	return chr;
}
void Shell_RPmsg_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_SHELL);
}
int main(int argc,char* argv[])
{
	(void)argc;(void)argv;
	StartupHook();

	SHELL_Mainloop();
	return 0;
}
