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
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
extern	TickType				OsTickCounter;
#if (KSM_NUM > 0)
static KSMState_Type ksm_state[KSM_NUM];
#endif
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

#if (KSM_NUM > 0)
void KsmInit(void)
{
	KsmID_Type i;
	for(i=0;i<KSM_NUM;i++)
	{
		ksm_state[i] = KSM_S_INVALID;
		KSM_Config[i].Ksm[KSM_S_INIT]();
	}
}
void KsmStart(void)
{
	KsmID_Type i;
	for(i=0;i<KSM_NUM;i++)
	{
		KSM_Config[i].Ksm[KSM_S_START]();
	}
}
void KsmStop(void)
{
	KsmID_Type i;
	for(i=0;i<KSM_NUM;i++)
	{
		KSM_Config[i].Ksm[KSM_S_STOP]();
	}
}
void KsmExecute(void)
{
	KsmID_Type i;
	for(i=0;i<KSM_NUM;i++)
	{
		if(ksm_state[i]<KSM_Config[i].KsLimit)
		{
			KSM_Config[i].Ksm[ksm_state[i]]();
		}
	}
}
void KsmSetState(KsmID_Type Ksm,KSMState_Type state)
{
	if(Ksm < KSM_NUM)
	{
		ksm_state[Ksm] = state;
	}
	else
	{
		asAssert(0);
	}
}
KSMState_Type KsmGetState(KsmID_Type Ksm)
{
	KSMState_Type state;
	if(Ksm < KSM_NUM)
	{
		state = ksm_state[Ksm];
	}
	else
	{
		state = KSM_S_INVALID;
		asAssert(0);
	}
	return state;
}
#endif /* KSM_NUM  */
void StartTimer(TimerType* timer)
{
	asAssert(timer!=NULL);
	*timer = OsTickCounter;
}
void StopTimer(TimerType* timer)
{
	asAssert(timer!=NULL);
	*timer = 0;
}
TimerType GetTimer(TimerType* timer)
{
	TimerType time;
	asAssert(timer!=NULL);

	if(0 == *timer)
	{
		time = 0;
	}
	else
	{
		if (OsTickCounter >= *timer)
		{
			 time = (OsTickCounter - *timer);
		}
		else
		{
			time = (TICK_MAX - *timer + OsTickCounter);
		}
	}
	return time;
}

#ifndef __POSIX_OSAL__
void __weak TaskIdleHook(void)
{

}
TASK(TaskIdle)
{
#if !defined(__SMALL_OS__)
	ASLOG(STDOUT,"TaskIdle is running\n");
#ifdef USE_PROTOTHREAD
	ASLOG(STDOUT,"Schedule Contiki in TaskIdle\n");
	StartContiki();
#endif
	for(;;)
	{
#else
#ifdef USE_PROTOTHREAD
	/* Yes, ugly, but I don't care */
	static uint32 flag = 0;
	if(0u == (flag&0x01))
	{
		ASLOG(STDOUT,"Schedule Contiki in TaskIdle\n");
		StartContiki();
		flag |= 0x01;
	}
#endif
#endif
		KSM_EXECUTE();
#ifdef USE_PROTOTHREAD
		ScheduleContiki();
#endif
#if defined(__FREEOSEK__) \
	|| defined(__UCOSII_OS__) \
	|| defined(__RTTHREAD_OS__) \
	|| defined(__ASKAR_OS__)
		(void)Schedule();
#endif

		TaskIdleHook();

#if !defined(__SMALL_OS__)
	}
#endif
}
#endif /* __POSIX_OSAL__ */

