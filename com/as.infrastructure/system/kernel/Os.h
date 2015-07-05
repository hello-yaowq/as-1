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
#include "Os_Cfg.h"
#include "Std_Types.h"
#include "ksm_cfg.h"
/* ============================ [ MACROS    ] ====================================================== */
#ifdef AS_OS_BASED_ON_TOPPERS_OSEK
#define OsActivateTask(x)	ActivateTask(TASK_ID_##x)
#define OsTerminateTask(x)	TerminateTask()
#endif /* AS_OS_BASED_ON_TOPPERS_OSEK */

/* KSM states */
#define KSM_S_INIT                0x00
#define KSM_S_START               0x01
#define KSM_S_STOP                0x02
#define KSM_S_INVALID             0xFF

#define TICK_MAX 					0xFFFFFFFF

#if(KSM_NUM > 0)
#define KSM_INIT() 		KsmInit()
#define KSM_EXECUTE() 	KsmExecute()
#else
#define KSM_INIT()
#define KSM_EXECUTE()
#endif

/* ============================ [ TYPES     ] ====================================================== */
#if(KSM_NUM < 0xFF)
typedef uint8 KsmID_Type;
#elif(KSM_NUM < 0xFFFF)
typedef uint16 KsmID_Type;
#else
typedef uint32 KsmID_Type;
#endif
/* Kernel Timer.
 * If Tick is 1ms per Tick,
 * then counter max time is (0xFFFFFFFF)ms = 49.71 days.
 */
typedef TickType TimerType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void StartOS( AppModeType app_mode );
extern void StartupHook( void );
extern void KsmInit(void);
extern void KsmStart(void);
extern void KsmStop(void);
extern void KsmExecute(void);
extern void KsmSetState(KsmID_Type Ksm,KSMState_Type state);
extern KSMState_Type KsmGetState(KsmID_Type Ksm);

extern void StartTimer(TimerType* timer);
extern void StopTimer(TimerType* timer);
extern TimerType GetTimer(TimerType* timer);
#endif /* COM_CLANG_INCLUDE_OS_H_ */
