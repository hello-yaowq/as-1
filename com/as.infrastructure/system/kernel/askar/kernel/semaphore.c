/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
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
#include "semaphore.h"
#include "kernel_internal.h"
#if(OS_PTHREAD_NUM > 0)
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	(void)pshared;

	TAILQ_INIT(&(sem->head));

	sem->value = value;

	return 0;
}

int sem_getvalue(sem_t *sem, int *sval)
{
	imask_t imask;

	Irq_Save(imask);
	*sval  = sem->value;
	Irq_Restore(imask);

	return 0;
}

int sem_destroy(sem_t *sem)
{
	(void)sem;

	return 0;
}

int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;

	Irq_Save(imask);

	if(sem->value > 0)
	{
		sem->value --;
	}
	else
	{
		ercd = Os_ListWait(&(sem->head), abstime);
	}

	Irq_Restore(imask);

	return ercd;
}

int sem_trywait(sem_t *sem)
{
	struct timespec tm = { 0, 0 };
	return sem_timedwait(sem, &tm);
}

int sem_wait(sem_t *sem)
{
	return sem_timedwait(sem, NULL);
}

int sem_post(sem_t *sem)
{
	int ercd = 0;
	imask_t imask;
	TaskVarType* pTaskVar;

	Irq_Save(imask);

	if(0 != Os_ListPost(&(sem->head), TRUE))
	{
		sem->value ++;
	}

	Irq_Restore(imask);

	return ercd;
}
#endif /* OS_PTHREAD_NUM > 0 */

