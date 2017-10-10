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
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	TaskType taskID;
	PriorityType priority;
} ReadyEntryType;

typedef struct
{
	uint32 size;
	ReadyEntryType heap[ACTIVATION_SUM];
} ReadyQueueType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static ReadyQueueType ReadyQueue;
/* ============================ [ LOCALS    ] ====================================================== */
static void Sched_BubbleUp(ReadyQueueType* pReadyQueue, uint32 index)
{
	uint32 father = index >> 1;
	while(pReadyQueue->heap[father].priority < pReadyQueue->heap[index].priority)
	{
		/*
		 * if the father priority is lower then the index priority, swap them
		 */
		ReadyEntryType tmpVar = pReadyQueue->heap[index];
		pReadyQueue->heap[index] = pReadyQueue->heap[father];
		pReadyQueue->heap[father] = tmpVar;
		index = father;
		father >>= 1;
	}
}

static void Sched_BubbleDown(ReadyQueueType* pReadyQueue, uint32 index)
{
	uint32 size = pReadyQueue->size;
	uint32 child;
	while ((child = index << 1) < size) /* child = left */
	{
		uint32 right = child + 1;
		if ((right < size) &&
			(pReadyQueue->heap[child].priority < pReadyQueue->heap[right].priority))
		{
			/* the right child exists and is greater */
			child = right;
		}
		if ((pReadyQueue->heap[index].priority < pReadyQueue->heap[child].priority))
		{
			/* the father has a priority <, swap */
			ReadyEntryType tmpVar = pReadyQueue->heap[index];
			pReadyQueue->heap[index] = pReadyQueue->heap[child];
			pReadyQueue->heap[child] = tmpVar;
			/* go down */
			index = child;
		}
		else
		{
			/* went down to its place, stop the loop */
			break;
		}
	}
}

/* ============================ [ FUNCTIONS ] ====================================================== */
void Sched_Init(void)
{
	ReadyQueue.size = 0;
}

void Sched_AddReady(TaskType TaskID)
{
	asAssert(ReadyQueue.size < ACTIVATION_SUM);

	ReadyQueue.heap[ReadyQueue.size].taskID = TaskID;
	ReadyQueue.heap[ReadyQueue.size].priority = TaskConstArray[TaskID].initPriority;
	Sched_BubbleUp(&ReadyQueue, ReadyQueue.size);
	ReadyQueue.size++;
	ReadyVar = &TaskVarArray[ReadyQueue.heap[0].taskID];
}

void Sched_Preempt(void)
{
	ReadyQueue.heap[0].taskID = RunningVar - TaskVarArray;
	ReadyQueue.heap[0].priority = RunningVar->priority;
}

void Sched_GetReady(void)
{
	if(ReadyQueue.size > 0)
	{
		ReadyVar = &TaskVarArray[ReadyQueue.heap[0].taskID];
		ReadyQueue.size --;
		ReadyQueue.heap[0] = ReadyQueue.heap[ReadyQueue.size];

		Sched_BubbleDown(&ReadyQueue, 0);
	}
	else
	{
		ReadyVar = NULL;
	}
}
