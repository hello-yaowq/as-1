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
#include "CDCDSerialDriver.h"
#else
#include "Lcd.h"
#include "Sg.h"
#endif

/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void StartupHook(void)
{
#ifdef CHIP_AT91SAM3S
	CDCDSerialDriver_Initialize();
#else
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
	Sg_Init();
#endif
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
			printf("ercd = %d E_OS_ACCESS!\r\n",ercd);
			break;
		case E_OS_CALLEVEL:
			printf("ercd = %d E_OS_CALLEVEL!\r\n",ercd);
			break;
		case E_OS_ID:
			printf("ercd = %d E_OS_ID!\r\n",ercd);
			break;
		case E_OS_LIMIT:
			printf("ercd = %d E_OS_LIMIT!\r\n",ercd);
			break;
		case E_OS_NOFUNC:
			printf("ercd = %d E_OS_NOFUNC!\r\n",ercd);
			break;
		case E_OS_RESOURCE:
			printf("ercd = %d E_OS_RESOURCE!\r\n",ercd);
			break;
		case E_OS_STATE:
			printf("ercd = %d E_OS_STATE!\r\n",ercd);
			break;
		case E_OS_VALUE	:
			printf("ercd = %d E_OS_VALUE!\r\n",ercd);
			break;
		default:
			printf("ercd = %d unknown error!\r\n",ercd);
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
