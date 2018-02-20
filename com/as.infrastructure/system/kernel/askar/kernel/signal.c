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
#include "signal.h"
#include "kernel_internal.h"
#if(OS_PTHREAD_NUM > 0)
#include <stdlib.h>
#include <stdarg.h>
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
struct signal
{
	int signum;
	struct sigaction action;
	TAILQ_ENTRY(signal) entry;
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static struct signal *lookup_signal(pthread_t tid, int signum)
{
	struct signal * sig;

	TAILQ_FOREACH(sig, &tid->signalList, entry)
	{
		if(sig->signum == signum)
		{
			break;
		}
	}

	return sig;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
int sigfillset (sigset_t *set)
{
	*set = (sigset_t)-1;
	return 0;
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	int ercd = 0;
	struct signal * sig;
	imask_t imask;
	pthread_t tid;

	asAssert((RunningVar-TaskVarArray) >= TASK_NUM);
	asAssert((RunningVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	tid = (pthread_t)(RunningVar->pConst);

	Irq_Save(imask);
	sig = lookup_signal(tid, signum);
	Irq_Restore(imask);

	if(NULL != sig)
	{
		if(oldact) *oldact = sig->action;
	}
	else
	{
		sig = malloc(sizeof(struct signal));
	}

	if(NULL != sig)
	{
		sig->signum = signum;
		sig->action = *act;
		Irq_Save(imask);
		TAILQ_INSERT_TAIL(&tid->signalList, sig, entry);
		Irq_Restore(imask);
	}
	else
	{
		ercd = ENOMEM;
	}

	return ercd;
}

int pthread_kill (pthread_t tid, int signum)
{
	int ercd = 0;
	struct signal * sig;
	imask_t imask;

	asAssert((tid->pTaskVar-TaskVarArray) >= TASK_NUM);
	asAssert((tid->pTaskVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));
	asAssert(tid->pTaskVar != RunningVar);

	Irq_Save(imask);
	sig = lookup_signal(tid, signum);
	Irq_Restore(imask);

	if(NULL != sig)
	{
		Irq_Save(imask);
		Os_PortInstallSignal(tid->pTaskVar, signum, sig->action.sa_handler);
		Irq_Restore(imask);
	}
	else
	{
		ercd = -EACCES;
	}

	return ercd;
}
#endif /* OS_PTHREAD_NUM */
