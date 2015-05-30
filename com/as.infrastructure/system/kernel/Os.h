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
#ifndef COM_CLANG_INCLUDE_OS_H_
#define COM_CLANG_INCLUDE_OS_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
#include "Os_Cfg.h"
/* ============================ [ MACROS    ] ====================================================== */
#ifdef AS_OS_BASED_ON_TOPPERS_OSEK
#define OsActivateTask(x)	ActivateTask(TASK_ID_##x)
#define OsTerminateTask(x)	TerminateTask()
#endif /* AS_OS_BASED_ON_TOPPERS_OSEK */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	void(*Init)(void);
	void(*DeInit)(void);
	void(*Run)(void);
}KSM_Type;	/* Kernel State Machine */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void StartOS( AppModeType app_mode );

extern void StartupHook( void );
#endif /* COM_CLANG_INCLUDE_OS_H_ */
