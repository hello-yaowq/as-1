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

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern int lua_main(int argc, char *argv[]);
/* ============================ [ DATAS     ] ====================================================== */
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

	SHELL_Init();
	SHELL_AddCmd(&luacmd);
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void Can_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	asAssert(chl == RPMSG_CHL_CAN);
}
void Can_RPmsg_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_CAN);
}
void Shell_RPmsg_RxNotitication(RPmsg_ChannelType chl,void* data, uint16 len)
{
	asAssert(chl == RPMSG_CHL_SHELL);
}
void Shell_RPmsg_TxConfirmation(RPmsg_ChannelType chl)
{
	asAssert(chl == RPMSG_CHL_SHELL);
}
int main(int argc,char* argv[])
{
	StartupHook();

	SHELL_Mainloop();
	return 0;
}
