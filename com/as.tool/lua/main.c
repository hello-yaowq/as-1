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
#include <pthread.h>
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

#ifdef USE_SHELL
#include "shell.h"
#endif
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define SHELL_CMD_CACHE_SIZE  4096
#define AS_LOG_SHELL 1
/* ============================ [ TYPES     ] ====================================================== */
#ifdef USE_SHELL
typedef struct {
	uint32_t r_pos;
	uint32_t w_pos;
	volatile uint32_t counter;
	pthread_mutex_t w_lock;
	char     cmd[SHELL_CMD_CACHE_SIZE];
}Shel_CmdInputCacheType;
#endif
/* ============================ [ DECLARES  ] ====================================================== */
extern int lua_main(int argc, char *argv[]);
#ifdef USE_SHELL
static int lua_main_entry(int argc, char *argv[]);
#endif
extern void luaclose_as(void);
/* ============================ [ DATAS     ] ====================================================== */
#ifdef USE_SHELL
static ShellCmdT luacmd =
{
	.func = lua_main_entry,
	.argMin = 0,
	.argMax = 0,
	.cmd = "lua",
	.shortDesc = "lua <script>",
	.longDesc ="lua script executor",
};
#endif

/* ============================ [ LOCALS    ] ====================================================== */
#ifdef USE_SHELL
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

	SHELL_Init();
	SHELL_AddCmd(&luacmd);
}
static int lua_main_entry(int argc, char *argv[])
{
	int rv = lua_main(argc,argv);
	luaclose_as();

	return rv;
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
#if defined(USE_RPMSG)
void RPmsg_Client_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	uint32_t i;
	char* cmd = (char*)data;
	asAssert(chl == RPMSG_CHL_CLIENT);
	ASLOG(SHELL,"receive cmd \"%s\"\n",cmd);
	/* for debug */
	/* strcpy(cmd,"lua d:/repository/as/com/as.tool/lua/script/flashloader.lua"); */
	if(NULL != strstr(cmd,"lua"))
	{

	}
	else
	{	/* invalid command, use default */
		strcpy(cmd,"lua d:/repository/as/com/as.tool/lua/script/flashloader.lua");
		len = strlen(cmd);
	}

	for(i=0;i<len;i++)
	{
		SHELL_input(cmd[i]);
	}
}
void RPmsg_Client_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_CLIENT);
}
#endif /* USE_RPMSG */
imask_t __Irq_Save(void) { return 0; }
void Irq_Restore(imask_t mask) { (void) mask; }
int main(int argc,char* argv[])
{
	ASENVINIT(argc,argv);
#ifdef USE_SHELL
	if( argc != 1 )
	{
		lua_main(argc,argv);
		luaclose_as();
	}
	else
	{
		StartupHook();

		SHELL_Mainloop();
	}
#else
	lua_main(argc,argv);
	luaclose_as();
#endif
	return 0;
}
