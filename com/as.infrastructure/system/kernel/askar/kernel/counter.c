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
#if (COUNTER_NUM > 0)
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
TickType				OsTickCounter;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
StatusType SignalCounter(CounterType CounterID)
{
	StatusType ercd = E_OK;
	imask_t imask;
	AlarmVarType *pVar;
	AlarmType AlarmID;

	if(CounterID < COUNTER_NUM)
	{
		Irq_Save(imask);
		/* yes, only software couter supported */
		CounterVarArray[CounterID].value++;
		#if (ALARM_NUM > 0)
		while(NULL != (pVar = TAILQ_FIRST(&CounterVarArray[CounterID].head))) /* intended '=' */
		{
			if (pVar->value == CounterVarArray[CounterID].value)
			{
				AlarmID = pVar - AlarmVarArray;
				TAILQ_REMOVE(&CounterVarArray[CounterID].head, &AlarmVarArray[AlarmID], entry);
				OS_STOP_ALARM(&AlarmVarArray[AlarmID]);
				AlarmConstArray[AlarmID].Action();

				if(AlarmVarArray[AlarmID].period != 0)
				{
					Os_StartAlarm(AlarmID,
						(TickType)(CounterVarArray[CounterID].value+AlarmVarArray[AlarmID].period),
						AlarmVarArray[AlarmID].period);
				}
			}
			else
			{
				break;
			}
		}
		#endif
		Irq_Restore(imask);
	}
	else
	{
		ercd = E_OS_ID;
	}
	
	return ercd;
}


TickType GetOsTick(void)
{
	return OsTickCounter;
}

void Os_CounterInit(void)
{
	CounterType id;
	OsTickCounter = 1;

	for(id=0; id < COUNTER_NUM; id++)
	{
		CounterVarArray[id].value = 0;
		TAILQ_INIT(&CounterVarArray[id].head);
	}
}
#endif /* #if (COUNTER_NUM > 0) */
