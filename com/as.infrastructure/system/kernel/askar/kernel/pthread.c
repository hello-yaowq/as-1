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
#include "kernel_internal.h"
#if(OS_PTHREAD_NUM > 0)
#include <errno.h>
#include <stdlib.h>
#include <asdebug.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
struct pthread
{
	TaskConstType TaskConst;
	TaskVarType* pTaskVar;
	TaskListType joinList;
	void *(*start) (void *);
	void* arg;
	void* ret;
};
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
	void* r;
	pthread_t pthread;

	pthread = (pthread_t)RunningVar->pConst;

	asAssert(pthread->pTaskVar == RunningVar);

	r = pthread->start(pthread->arg);

	pthread_exit(r);

}

static boolean pthread_CheckAccess(ResourceType ResID)
{
	(void) ResID;
	/* not allowd to access any OSEK resource */
	return FALSE;
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
		memset(pTaskVar, 0, sizeof(TaskVarType));
		if((NULL != attr) && (NULL != attr->stack_base))
		{	/* to create it totally static by using stack to allocate pthread */
			pthread = (pthread_t)attr->stack_base;
			pthread->pTaskVar = pTaskVar;
			pTaskConst = &(pthread->TaskConst);

			pTaskConst->pStack = attr->stack_base+sizeof(struct pthread);
			pTaskConst->stackSize = attr->stack_size - sizeof(struct pthread);
			pTaskConst->initPriority = attr->priority;
			pTaskConst->runPriority = attr->priority;
			pTaskConst->flag = 0;
			asAssert(attr->priority < OS_PTHREAD_PRIORITY);
		}
		else
		{
			pthread = malloc(PTHREAD_DEFAULT_STACK_SIZE+sizeof(struct pthread));
			if(NULL == pthread)
			{
				free(pthread);
				ercd = -ENOMEM;
			}
			else
			{
				pthread->pTaskVar = pTaskVar;
				pTaskConst = &(pthread->TaskConst);
				pTaskConst->pStack = ((void*)pthread)+sizeof(struct pthread);
				pTaskConst->stackSize = PTHREAD_DEFAULT_STACK_SIZE;
				pTaskConst->initPriority = PTHREAD_DEFAULT_PRIORITY;
				pTaskConst->runPriority = PTHREAD_DEFAULT_PRIORITY;
				pTaskConst->flag = PTHREAD_DYNAMIC_CREATED_MASK;
			}
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
		pTaskConst->CheckAccess = pthread_CheckAccess;
		pTaskConst->name = "pthread";
		#ifdef MULTIPLY_TASK_ACTIVATION
		pTaskConst->maxActivation = 1;
		#endif
		pTaskVar->state = READY;
		memset(pTaskConst->pStack, 0, pTaskConst->stackSize);

		pTaskVar->pConst = pTaskConst;
		pTaskVar->priority = pTaskConst->initPriority;
		Os_PortInitContext(pTaskVar);

		TAILQ_INIT(&pthread->joinList);

		Irq_Save(imask);
		if((NULL == attr) || (PTHREAD_CREATE_JOINABLE == attr->detachstate))
		{
			pTaskConst->flag |= PTHREAD_JOINABLE_MASK;
		}
		Sched_PosixAddReady(pTaskVar - TaskVarArray);
		Irq_Restore(imask);
	}

	return ercd;
}

void pthread_exit (void *value_ptr)
{
	pthread_t tid;
	TaskVarType *pTaskVar;

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);
	asAssert((RunningVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	tid = (pthread_t)(RunningVar->pConst);

	Irq_Disable();

	if(tid->TaskConst.flag & PTHREAD_JOINABLE_MASK)
	{
		tid->TaskConst.flag |= PTHREAD_JOINED_MASK;
		if(0 == Os_ListPost(&tid->joinList, FALSE))
		{
			asAssert(TAILQ_EMPTY(&tid->joinList));
		}
	}
	else
	{
		tid->pTaskVar->pConst = NULL;
		if(tid->TaskConst.flag & PTHREAD_DYNAMIC_CREATED_MASK)
		{
			free(tid);
		}
	}

	Sched_GetReady();
	Os_PortDispatch();
}

int pthread_detach(pthread_t tid)
{
	int ercd = 0;
	TaskVarType *pTaskVar;
	imask_t imask;

	asAssert(tid);
	asAssert((tid->pTaskVar-TaskVarArray) >= TASK_NUM);
	asAssert((tid->pTaskVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	Irq_Save(imask);
	if(tid->TaskConst.flag & PTHREAD_JOINED_MASK)
	{	/* the tid is already exited */
		tid->pTaskVar->pConst = NULL;
		if(tid->TaskConst.flag & PTHREAD_DYNAMIC_CREATED_MASK)
		{
			free(tid);
		}
	}
	else
	{
		tid->TaskConst.flag &= ~PTHREAD_JOINABLE_MASK;
	}
	Irq_Restore(imask);

	return ercd;
}

int pthread_join(pthread_t tid, void ** thread_return)
{
	int ercd = 0;
	TaskVarType *pTaskVar;
	imask_t imask;

	asAssert(tid);
	asAssert((tid->pTaskVar-TaskVarArray) >= TASK_NUM);
	asAssert((tid->pTaskVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	Irq_Save(imask);
	if(tid->TaskConst.flag & PTHREAD_JOINABLE_MASK)
	{
		if(0u == (tid->TaskConst.flag & PTHREAD_JOINED_MASK))
		{
			(void)Os_ListWait(&tid->joinList, NULL);
		}
		asAssert(tid->TaskConst.flag & PTHREAD_JOINED_MASK);
		tid->pTaskVar->pConst = NULL;
		if(tid->TaskConst.flag & PTHREAD_DYNAMIC_CREATED_MASK)
		{
			free(tid);
		}
	}
	else
	{
		ercd = -EACCES;
	}
	Irq_Restore(imask);

	return ercd;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	(void)attr;

	TAILQ_INIT(&(mutex->head));

	mutex->locked = FALSE;

	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	(void)mutex;
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	int ercd = 0;
	imask_t imask;

	Irq_Save(imask);

	if(TRUE == mutex->locked)
	{
		/* wait it forever */
		Os_ListWait(&mutex->head, NULL);
	}

	mutex->locked = TRUE;


	Irq_Restore(imask);

	return ercd;
}
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;

	Irq_Save(imask);

	if(TRUE == mutex->locked)
	{
		if(0 != Os_ListPost(&mutex->head, TRUE))
		{
			mutex->locked = FALSE;
		}
	}
	else
	{
		ercd = -EACCES;
	}

	Irq_Restore(imask);

	return ercd;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	int ercd = 0;
	imask_t imask;

	Irq_Save(imask);

	if(TRUE == mutex->locked)
	{
		ercd = -EBUSY;
	}
	else
	{
		mutex->locked = TRUE;
	}

	Irq_Restore(imask);

	return ercd;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
	(void)attr;

	TAILQ_INIT(&(cond->head));

	cond->signals = 0;

	return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
	(void)cond;
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
	TaskVarType* pTaskVar;
	TaskVarType *pNext;
	imask_t imask;

	Irq_Save(imask);

	while(0 == Os_ListPost(&(cond->head), FALSE))
		;

	(void) Schedule();

	Irq_Restore(imask);

	return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;

	Irq_Save(imask);

	if(0 != Os_ListPost(&cond->head, TRUE))
	{
		cond->signals ++;
	}

	Irq_Restore(imask);

	return ercd;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	return pthread_cond_timedwait(cond, mutex, NULL);

}
int pthread_cond_timedwait(pthread_cond_t        *cond,
                           pthread_mutex_t       *mutex,
                           const struct timespec *abstime)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;
	TickType ticks;

	asAssert(mutex->locked);

	Irq_Save(imask);

	if(0 == cond->signals)
	{
		ercd = pthread_mutex_unlock(mutex);

		if(0 == ercd)
		{
			ercd = Os_ListWait(&(cond->head), abstime);

			(void)pthread_mutex_lock(mutex);
		}
	}
	else
	{
		cond->signals --;
	}

	Irq_Restore(imask);

	return ercd;
}
#endif
