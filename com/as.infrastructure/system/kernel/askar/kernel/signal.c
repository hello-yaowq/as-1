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
#ifdef USE_PTHREAD_SIGNAL
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
void Os_FreeSignalHandler(pthread_t tid)
{
	struct signal * sig;
	struct signal * next;

	sig = TAILQ_FIRST(&tid->signalList);
	while(NULL != sig)
	{
		next = TAILQ_NEXT(sig, entry);
		TAILQ_REMOVE(&tid->signalList, sig, entry);
		free(sig);
		sig = next;
	}

}

void Os_SignalInit(void)
{

}

int sigaddset (sigset_t *set, const int signo)
{
	int ercd = 0;

	if(signo < NSIG)
	{
		*set |= (1<<signo);
	}
	else
	{
		ercd = -EINVAL;
	}

	return ercd;
}

int sigdelset (sigset_t *set, const int signo)
{
	int ercd = 0;

	if(signo < NSIG)
	{
		*set &= ~(1<<signo);
	}
	else
	{
		ercd = -EINVAL;
	}

	return ercd;
}

int sigemptyset (sigset_t *set)
{
	*set = (sigset_t)0;
	return 0;
}

int sigfillset (sigset_t *set)
{
	*set = (sigset_t)-1;
	return 0;
}

int sigismember (const sigset_t *set, int signo)
{
	int r = 0;

	if(signo < NSIG)
	{
		if(*set & (1<<signo))
		{
			r = 1;
		}
	}
	else
	{
		r = -EINVAL;
	}

	return r;
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	int ercd = 0;
	struct signal * sig;
	imask_t imask;
	pthread_t tid;

	if(signum >= NSIG)
	{
		return -EINVAL;
	}

	tid = pthread_self();

	Irq_Save(imask);
	sig = lookup_signal(tid, signum);
	Irq_Restore(imask);

	if(NULL != sig)
	{
		if(oldact) *oldact = sig->action;
	}

	if(NULL == sig)
	{
		if(NULL != act)
		{
			asAssert(act->sa_handler);
			sig = malloc(sizeof(struct signal));
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
		}
	}
	else
	{
		if(NULL != act)
		{	/* replace old action */
			asAssert(act->sa_handler);
			Irq_Save(imask);
			sig->action = *act;
			Irq_Restore(imask);
		}
	}

	return ercd;
}

sighandler_t signal (int signum, sighandler_t action)
{
	sighandler_t r = SIG_ERR;
	int ercd;
	struct sigaction act,oldact;

	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = action;
	oldact.sa_handler = SIG_IGN;

	ercd = sigaction(signum, &act, &oldact);
	if(0 == ercd)
	{
		r = oldact.sa_handler;
	}

	return r;
}

int sigwait(const sigset_t * set, int * sig)
{
	int ercd = 0;

	imask_t imask;
	pthread_t tid;

	asAssert((NULL != set) && (NULL != sig));

	if(0 != *set)
	{
		tid = pthread_self();

		Irq_Save(imask);
		tid->sigWait = *set;
		(void)Os_ListWait(&tid->sigList, NULL);
		*sig = tid->signo;
		Irq_Restore(imask);
	}
	else
	{
		ercd = -EINVAL;
	}

	return ercd;
}

int pthread_kill (pthread_t tid, int signum)
{
	int ercd = 0;
	struct signal * sig = NULL;
	imask_t imask;

	if(signum >= NSIG)
	{
		return -EINVAL;
	}

	asAssert((tid->pTaskVar-TaskVarArray) >= TASK_NUM);
	asAssert((tid->pTaskVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	if(NULL != tid->pTaskVar->pConst)
	{
		Irq_Save(imask);
		sig = lookup_signal(tid, signum);
		Irq_Restore(imask);
		if(NULL != sig)
		{
			if(tid->pTaskVar != RunningVar)
			{
				Irq_Save(imask);
				Os_PortInstallSignal(tid->pTaskVar, signum, sig->action.sa_handler);
				Irq_Restore(imask);
			}
			else
			{
				sig->action.sa_handler(signum);
			}
		}

		tid->signo = signum;

		if(tid->sigWait & (1<<signum))
		{
			Irq_Save(imask);
			Os_ListPost(&tid->sigList, TRUE);
			Irq_Restore(imask);
		}
	}
	else
	{	/* not pthread type task */
		ercd = -EACCES;
	}

	return ercd;
}

int raise (int sig)
{
	return pthread_kill(pthread_self(), sig);
}
#endif /* USE_PTHREAD_SIGNAL */
#endif /* OS_PTHREAD_NUM */
