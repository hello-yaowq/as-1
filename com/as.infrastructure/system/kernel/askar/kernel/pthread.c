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
#include <unistd.h>
/* ============================ [ MACROS    ] ====================================================== */
#define DYNAMIC_CREATED_PTHREAD(pConst) (pTaskConst->autoStart)
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#if(OS_PTHREAD_NUM > 0)
extern TAILQ_HEAD(sleep_list, TaskVar) OsSleepListHead;
#endif
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

		if((NULL != attr) && (NULL != attr->stack_base))
		{	/* to create it totally static by using stack to allocate pthread */
			pthread = (pthread_t)attr->stack_base;
			pthread->pTaskVar = pTaskVar;
			pTaskConst = &(pthread->TaskConst);

			pTaskConst->pStack = attr->stack_base+sizeof(struct pthread);
			pTaskConst->stackSize = attr->stack_size - sizeof(struct pthread);
			pTaskConst->initPriority = attr->priority;
			pTaskConst->runPriority = attr->priority;
			DYNAMIC_CREATED_PTHREAD(pTaskConst) = FALSE;
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
				DYNAMIC_CREATED_PTHREAD(pTaskConst) = TRUE;
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

		pTaskVar->pConst = pTaskConst;
		pTaskVar->priority = pTaskConst->initPriority;
		Os_PortInitContext(pTaskVar);

		Irq_Save(imask);
		Sched_PosixAddReady(pTaskVar - TaskVarArray);
		Irq_Restore(imask);
	}

	return ercd;
}

void pthread_exit (void *value_ptr)
{
	TaskConstType* pTaskConst;

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);

	pTaskConst = (TaskConstType*)RunningVar->pConst;

	Irq_Disable();

	RunningVar->pConst = NULL;

	if(TRUE == DYNAMIC_CREATED_PTHREAD(pTaskConst))
	{
		free(pTaskConst);
	}

	Sched_GetReady();
	Os_PortDispatch();
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

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);

	Irq_Save(imask);

	if(TRUE == mutex->locked)
	{
		/* wait it forever */
		RunningVar->state |= PTHREAD_STATE_WAITING;;
		TAILQ_INSERT_TAIL(&(mutex->head), RunningVar, entry);
		Sched_GetReady();
		Os_PortDispatch();
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

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);

	Irq_Save(imask);

	if(TRUE == mutex->locked)
	{
		if(FALSE == TAILQ_EMPTY(&(mutex->head)))
		{
			pTaskVar = TAILQ_FIRST(&(mutex->head));
			TAILQ_REMOVE(&(mutex->head), pTaskVar, entry);
			pTaskVar->state &= ~PTHREAD_STATE_WAITING;;
			OS_TRACE_TASK_ACTIVATION(pTaskVar);
			Sched_PosixAddReady(pTaskVar-TaskVarArray);
		}
		else
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

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);

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

	pTaskVar = TAILQ_FIRST(&(cond->head));
	while(NULL != pTaskVar)
	{
		pTaskVar->state &= ~PTHREAD_STATE_WAITING;
		OS_TRACE_TASK_ACTIVATION(pTaskVar);
		Sched_PosixAddReady(pTaskVar-TaskVarArray);

		pNext = TAILQ_NEXT(pTaskVar, entry);
		TAILQ_REMOVE(&(cond->head), pTaskVar, entry);
		pTaskVar = pNext;
	}

	Irq_Restore(imask);

	return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);

	Irq_Save(imask);

	if(FALSE == TAILQ_EMPTY(&(cond->head)))
	{
		pTaskVar = TAILQ_FIRST(&(cond->head));
		TAILQ_REMOVE(&(cond->head), pTaskVar, entry);
		pTaskVar->state &= ~PTHREAD_STATE_WAITING;
		OS_TRACE_TASK_ACTIVATION(pTaskVar);
		Sched_PosixAddReady(pTaskVar-TaskVarArray);
	}
	else
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

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);
	asAssert(mutex->locked);

	Irq_Save(imask);

	if(0 == cond->signals)
	{
		/* do unlock */
		if(FALSE == TAILQ_EMPTY(&(mutex->head)))
		{
			pTaskVar = TAILQ_FIRST(&(mutex->head));
			TAILQ_REMOVE(&(mutex->head), pTaskVar, entry);
			pTaskVar->state &= ~PTHREAD_STATE_WAITING;
			OS_TRACE_TASK_ACTIVATION(pTaskVar);
			Sched_PosixAddReady(pTaskVar-TaskVarArray);
		}
		else
		{
			mutex->locked = FALSE;
		}

		/* do cond signal wait */
		RunningVar->state |= PTHREAD_STATE_WAITING;
		TAILQ_INSERT_TAIL(&(cond->head), RunningVar, entry);

		if(NULL != abstime)
		{
			ticks = (abstime->tv_sec*1000000 + abstime->tv_nsec/1000 + USECONDS_PER_TICK-1)/USECONDS_PER_TICK;
			RunningVar->sleep_tick = ticks;
			RunningVar->state |= PTHREAD_STATE_SLEEPING;
			TAILQ_INSERT_TAIL(&OsSleepListHead, RunningVar, sentry);
		}

		Sched_GetReady();
		Os_PortDispatch();

		/* wakeup */
		if(RunningVar->state&PTHREAD_STATE_WAITING)
		{	/* this is timeout */
			TAILQ_REMOVE(&(cond->head), RunningVar, entry);
			ercd = -ETIMEDOUT;
		}
		else if(RunningVar->state&PTHREAD_STATE_SLEEPING)
		{	/* signal reached before timeout */
			TAILQ_REMOVE(&OsSleepListHead, RunningVar, sentry);
		}
		else
		{
			/* do nothing */
		}

		/* do lock again */
		if(TRUE == mutex->locked)
		{
			/* wait it forever */
			RunningVar->state |= PTHREAD_STATE_WAITING;
			TAILQ_INSERT_TAIL(&(mutex->head), RunningVar, entry);
			Sched_GetReady();
			Os_PortDispatch();
		}

		mutex->locked = TRUE;

	}
	else
	{
		cond->signals --;
	}

	Irq_Restore(imask);

	return ercd;
}
int usleep (unsigned long __useconds)
{
	Os_Sleep((__useconds+USECONDS_PER_TICK-1)/USECONDS_PER_TICK);
	return __useconds;
}
#endif
