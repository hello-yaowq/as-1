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
#include "osek_kernel.h"
#include "task.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

void disable_int(void)
{
}
void enable_int(void)
{
}

void dispatch(void)
{
}
void set_ipl(IPL ipl)
{
}
IPL  current_ipl(void)
{
	return 0;
}
void start_dispatch(void)
{
}
void exit_and_dispatch(void)
{
}
void activate_context(TaskType TaskID)
{
}

void cpu_initialize(void)
{

}
void sys_initialize(void)
{

}
void tool_initialize(void)
{

}

void EnterISR2(void)
{
}
void ExitISR2(void)
{
}

void cpu_terminate(void)
{

}
void sys_exit(void)
{
}

