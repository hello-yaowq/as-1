/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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
#include "kernel_internal.h"
#ifdef USE_SHELL
#include "shell.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#ifdef USE_SHELL
static int statOsFunc(int argc, char* argv[]);
extern void statOsTask(void);
extern void statOsAlarm(void);
#endif
/* ============================ [ DATAS     ] ====================================================== */
OSServiceIdType _errorhook_svcid;
_ErrorHook_Par  _errorhook_par1, _errorhook_par2, _errorhook_par3;

TaskVarType* RunningVar;
TaskVarType* ReadyVar;
unsigned int CallLevel;
static AppModeType appMode;
#ifdef USE_SHELL
static SHELL_CONST ShellCmdT statOsCmd  = {
	statOsFunc,
	0,0,
	"stat",
	"stat <task/alarm/counter>",
	"Show the status of operationg system\n",
	{NULL,NULL}
};
SHELL_CMD_EXPORT(statOsCmd);
#endif
/* ============================ [ LOCALS    ] ====================================================== */
static void Os_MiscInit(void)
{
	RunningVar = NULL;
	ReadyVar   = NULL;
	CallLevel  = TCL_NULL;

	Sched_Init();
#if defined(USE_SHELL) && !defined(__GNUC__)
	SHELL_AddCmd(&statOsCmd);
#endif
}
#ifdef USE_SHELL
static int statOsFunc(int argc, char* argv[])
{
	if((1 == argc) || (0 == strcmp(argv[1],"task")))
	{
		statOsTask();
	}

	if((1 == argc) || (0 == strcmp(argv[1],"alarm")))
	{
		statOsAlarm();
	}
	return 0;
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
/* |------------------+------------------------------------------------------| */
/* | Syntax:          | void StartOS ( AppModeType <Mode> )                  | */
/* |------------------+------------------------------------------------------| */
/* | Parameter (In):  | Mode:application mode                                | */
/* |------------------+------------------------------------------------------| */
/* | Parameter (Out): | none                                                 | */
/* |------------------+------------------------------------------------------| */
/* | Description:     | The user can call this system service to start the   | */
/* |                  | operating system in a specific mode, see chapter 5   | */
/* |                  | (os223.doc), Application modes.                      | */
/* |------------------+------------------------------------------------------| */
/* | Particularities: | Only allowed outside of the operating system,        | */
/* |                  | therefore implementation specific restrictions may   | */
/* |                  | apply. See also chapter 11.3, System start-up,       | */
/* |                  | especially with respect to systems where OSEK and    | */
/* |                  | OSEKtime coexist. This call does not need to return. | */
/* |------------------+------------------------------------------------------| */
/* | Conformance:     | BCC1, BCC2, ECC1, ECC2                               | */
/* |------------------+------------------------------------------------------| */
void StartOS ( AppModeType Mode )
{
	appMode = Mode;

	Irq_Disable();

	Os_MiscInit();
	Os_PortInit();
	Os_TaskInit();
	Os_ResourceInit();
#if (COUNTER_NUM > 0)
	Os_CounterInit();
#endif
#if (ALARM_NUM > 0)
	Os_AlarmInit();
#endif

	OSStartupHook();

	Sched_GetReady();
	Os_PortStartDispatch();
	while(1);
}

void ShutdownOS( StatusType Error )
{
	OSShutdownHook(Error);
	Irq_Disable();
	while(1);
}

AppModeType GetActiveApplicationMode ( void )
{
	return appMode;
}
