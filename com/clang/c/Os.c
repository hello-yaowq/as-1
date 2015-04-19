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

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void OsTaskInit(void);
extern void OsAlarmInit(void);
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

void StartOS( AppModeType app_mode )
{
	OsTaskInit();
	OsAlarmInit();

	vTaskStartScheduler();
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
	_assert(__FUNCTION__, pcFileName, ulLine);
}

unsigned long ulGetRunTimeCounterValue( void )
{
	return 0;
}

void vConfigureTimerForRunTimeStats( void )
{

}
