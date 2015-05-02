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
#include "Os.h"
#include "Lcd.h"
#include "Sg.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void StartupHook(void)
{
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
	Sg_Init();
}

TASK(TaskApp)
{
	OsTerminateTask(TaskApp);
}
TASK(TaskCom)
{
	OsTerminateTask(TaskCom);
}
TASK(TaskSg)
{
	Sg_ManagerTask();
	OsTerminateTask(TaskSg);
}

ALARM(Alarm5ms)
{
	OsActivateTask(TaskSg);
	OsActivateTask(TaskCom);
}
ALARM(Alarm10ms)
{
}
ALARM(Alarm20ms)
{
	OsActivateTask(TaskApp);
}
ALARM(Alarm50ms)
{
}
ALARM(Alarm100ms)
{
}

void ErrorHook(StatusType ercd)
{

}
void PreTaskHook(void)
{

}
void PostTaskHook(void)
{

}
void ShutdownHook(StatusType ercd)
{

}
