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
	uint32 size;
	TaskVarType* heap[ACTIVATION_SUM];
} ReadyQueueType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static ReadyQueueType ReadyQueue;
/* ============================ [ LOCALS    ] ====================================================== */
static void Sched_BubbleUp(ReadyQueueType* pReadyQueue, uint32 index)
{
	uint32 father = index >> 1;
	while(pReadyQueue->heap[father]->priority < pReadyQueue->heap[index]->priority)
	{
		/*
		 * if the father priority is lower then the index key, swap them
		 */
		TaskVarType* tmpVar = pReadyQueue->heap[index];
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
	while ((child = index << 1) <= size) /* child = left */
	{
		uint32 right = child + 1;
		if ((right <= size) &&
			(pReadyQueue->heap[child]->priority < pReadyQueue->heap[right]->priority))
		{
			/* the right child exists and is greater */
			child = right;
		}
		if ((pReadyQueue->heap[index]->priority < pReadyQueue->heap[child]->priority))
		{
			/* the father has a key <, swap */
			TaskVarType* tmpVar = pReadyQueue->heap[index];
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

void Sched_AddReady(TaskVarType* pTaskVar)
{

	TaskVarType* tmpVar;

	if(NULL == ReadyVar)
	{
		ReadyVar = pTaskVar;
	}
	else
	{
		if(pTaskVar->priority > ReadyVar->priority)
		{
			ReadyVar = pTaskVar;
		}
	}

	asAssert(ReadyQueue.size < ACTIVATION_SUM);

	ReadyQueue.heap[ReadyQueue.size] = pTaskVar;
	Sched_BubbleUp(&ReadyQueue, ReadyQueue.size);
	ReadyQueue.size++;

}

void Sched_Preempt(void)
{
	ReadyQueue.heap[0] = RunningVar;
}

void Sched_GetReady(void)
{
	if(ReadyQueue.size > 0)
	{
		ReadyVar = ReadyQueue.heap[0];
		ReadyQueue.size --;
		ReadyQueue.heap[0] = ReadyQueue.heap[ReadyQueue.size];

		Sched_BubbleDown(&ReadyQueue, 0);
	}
	else
	{
		ReadyVar = NULL;
	}
}
