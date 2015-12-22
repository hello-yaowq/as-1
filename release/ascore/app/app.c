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
#ifdef USE_GUI
#include "Sg.h"
#include "Lcd.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void StartupHook(void)
{
#ifdef USE_GUI
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
	Sg_Init();
	OsSetRelAlarm(AlarmApp, 10, 100);
#endif
}

TASK(TaskApp)
{
#ifdef USE_GUI
	Sg_ManagerTask();
#endif

	OsTerminateTask(TaskApp);
}

ALARM(AlarmApp)
{
	OsActivateTask(TaskApp);
}

void ErrorHook(StatusType ercd)
{

	switch(ercd)
	{
		case E_OS_ACCESS:
			ASLOG(OS,"ercd = %d E_OS_ACCESS!\r\n",ercd);
			break;
		case E_OS_CALLEVEL:
			ASLOG(OS,"ercd = %d E_OS_CALLEVEL!\r\n",ercd);
			break;
		case E_OS_ID:
			ASLOG(OS,"ercd = %d E_OS_ID!\r\n",ercd);
			break;
		case E_OS_LIMIT:
			ASLOG(OS,"ercd = %d E_OS_LIMIT!\r\n",ercd);
			break;
		case E_OS_NOFUNC:
			ASLOG(OS,"ercd = %d E_OS_NOFUNC!\r\n",ercd);
			break;
		case E_OS_RESOURCE:
			ASLOG(OS,"ercd = %d E_OS_RESOURCE!\r\n",ercd);
			break;
		case E_OS_STATE:
			ASLOG(OS,"ercd = %d E_OS_STATE!\r\n",ercd);
			break;
		case E_OS_VALUE	:
			ASLOG(OS,"ercd = %d E_OS_VALUE!\r\n",ercd);
			break;
		default:
			ASLOG(OS,"ercd = %d unknown error!\r\n",ercd);
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
#if defined(__FREERTOS__) || defined(__FREEOSEK__)
void PreTaskHook(void)
{
}
void PostTaskHook(void)
{
}
#else
extern TaskType	runtsk;
void PreTaskHook(void)
{
	ASLOG(OS,"PreTaskHook(%d)\n",runtsk);
}
void PostTaskHook(void)
{
	ASLOG(OS,"PostTaskHook(%d)\n",runtsk);
}
#endif
void ShutdownHook(StatusType ercd)
{
}

