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
#include "pthread.h"
#if(OS_PTHREAD_NUM > 0)
#include <errno.h>
#include <stdlib.h>
#include <asdebug.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static TaskVarType* pthread_malloc_tcb(void)
{
	TaskType id;

	TaskVarType* pTaskVar = NULL;

	for(id=0; id < OS_PTHREAD_NUM; id++)
	{
		if(NULL == TaskVarArray[TASK_NUM+id].pConst)
		{
			pTaskVar = &TaskVarArray[TASK_NUM+id];
			pTaskVar->pConst = (void*)1; /* reservet it */
			break;
		}
	}

	return pTaskVar;
}
static void pthread_entry_main(void)
{
	pthread_t pthread;

	pthread = (pthread_t)RunningVar->pConst;

	asAssert(pthread->pTaskVar == RunningVar);

	pthread->start(pthread->arg);

}
/* ============================ [ FUNCTIONS ] ====================================================== */
int pthread_create (pthread_t *tid, const pthread_attr_t *attr,
    void *(*start) (void *), void *arg)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;
	TaskConstType* pTaskConst;
	pthread_t pthread;

	Irq_Save(imask);

	pTaskVar = pthread_malloc_tcb();

	Irq_Restore(imask);

	if(NULL != pTaskVar)
	{
		pthread = malloc(sizeof(struct pthread));
		if(NULL != pthread)
		{
			pthread->pTaskVar = pTaskVar;
			pTaskConst = &(pthread->TaskConst);

			if((NULL != attr) && (NULL != attr->stack_base))
			{
				pTaskConst->pStack = attr->stack_base;
				pTaskConst->stackSize = attr->stack_size;
				pTaskConst->initPriority = attr->priority;
				pTaskConst->runPriority = attr->priority;
				asAssert(attr->priority < OS_PTHREAD_PRIORITY);
			}
			else
			{
				pTaskConst->pStack = malloc(PTHREAD_DEFAULT_STACK_SIZE);
				if(NULL == pTaskConst->pStack)
				{
					free(pthread);
					ercd = -ENOMEM;
				}
				else
				{
					pTaskConst->stackSize = PTHREAD_DEFAULT_STACK_SIZE;
					pTaskConst->initPriority = PTHREAD_DEFAULT_PRIORITY;
					pTaskConst->runPriority = PTHREAD_DEFAULT_PRIORITY;
				}
			}
		}
		else
		{
			ercd = -ENOMEM;
		}
	}
	else
	{
		ercd = -ENOMEM;
	}

	if(NULL != tid)
	{
		*tid = pthread;
	}

	if(0 == ercd)
	{
		pthread->arg = arg;
		pthread->start = start;
		pTaskConst->entry = pthread_entry_main;

		pTaskVar->pConst = pTaskConst;
		pTaskVar->priority = pTaskConst->initPriority;
		Os_PortInitContext(pTaskVar);

		Irq_Save(imask);
		Sched_PosixAddReady(pTaskVar - TaskVarArray);
		Irq_Restore(imask);
	}

	return ercd;
}
#endif
