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
#ifdef CHIP_AT91SAM3S
#include "board.h"
#else
#include "Lcd.h"
#endif
#include "Sg.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void StartupHook(void)
{
#ifdef CHIP_AT91SAM3S
//	LCDD_Initialize();
//	LCDD_On();
#else
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
        Sg_Init();
#endif
}

TASK(TaskApp)
{
	//printf("TaskApp is running!\n");
	OsTerminateTask(TaskApp);
}
TASK(TaskCom)
{
	OsTerminateTask(TaskCom);
}
TASK(TaskSg)
{
#ifdef CHIP_AT91SAM3S
#else  
	Sg_ManagerTask();
#endif
	OsTerminateTask(TaskSg);
}

TASK(TaskIdle)
{
	for(;;);
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

	switch(ercd)
	{
		case E_OS_ACCESS:
			printf("ercd = %d E_OS_ACCESS!\n",ercd);
			break;
		case E_OS_CALLEVEL:
			printf("ercd = %d E_OS_CALLEVEL!\n",ercd);
			break;
		case E_OS_ID:
			printf("ercd = %d E_OS_ID!\n",ercd);
			break;
		case E_OS_LIMIT:
			printf("ercd = %d E_OS_LIMIT!\n",ercd);
			break;
		case E_OS_NOFUNC:
			printf("ercd = %d E_OS_NOFUNC!\n",ercd);
			break;
		case E_OS_RESOURCE:
			printf("ercd = %d E_OS_RESOURCE!\n",ercd);
			break;
		case E_OS_STATE:
			printf("ercd = %d E_OS_STATE!\n",ercd);
			break;
		case E_OS_VALUE	:
			printf("ercd = %d E_OS_VALUE!\n",ercd);
			break;
		default:
			printf("ercd = %d unknown error!\n",ercd);
			break;
	}

	if(E_OS_CALLEVEL == ercd)
	{
		ShutdownOS(E_OS_CALLEVEL);
	}
	else
	{
		/* recover-able error */
	}
}
void PreTaskHook(void)
{

}
void PostTaskHook(void)
{

}
void ShutdownHook(StatusType ercd)
{
	for(;;);
}
