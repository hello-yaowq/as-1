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
#define ENABLE_FIFO_SCHED
#ifdef ENABLE_FIFO_SCHED
/* ============================ [ INCLUDES  ] ====================================================== */
#include "kernel_internal.h"
/* ============================ [ MACROS    ] ====================================================== */
#define SCHED_FIFO_SIZE(fifo) (fifo)[0]
#define SCHED_FIFO_HEAD(fifo) (fifo)[1]
#define SCHED_FIFO_TAIL(fifo) (fifo)[2]

#define SCHED_FIFO_SLOT_OFFSET 3
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
extern const ReadyFIFOType ReadyFIFO[PRIORITY_NUM+1];
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void Sched_Init(void)
{
	PriorityType prio;

	for(prio=0; prio <= PRIORITY_NUM; prio++)
	{
		if(ReadyFIFO[prio].pFIFO != NULL)
		{
			SCHED_FIFO_SIZE(ReadyFIFO[prio].pFIFO) = 0;
			SCHED_FIFO_HEAD(ReadyFIFO[prio].pFIFO) = SCHED_FIFO_SLOT_OFFSET;
			SCHED_FIFO_TAIL(ReadyFIFO[prio].pFIFO) = SCHED_FIFO_SLOT_OFFSET;
		}
	}
}

void Sched_AddReady(TaskType TaskID)
{

}
void Sched_Preempt(void)
{
	OSPostTaskHook();
}

void Sched_GetReady(void)
{

}

bool Sched_Schedule(void)
{
	bool needSchedule = FALSE;

	return needSchedule;
}
#endif /* ENABLE_FIFO_SCHED */
