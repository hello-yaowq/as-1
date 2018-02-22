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
static void sig_default_hadler(int signum);
/* ============================ [ DATAS     ] ====================================================== */
static const struct signal sig_default =
{
	.action.sa_handler = sig_default_hadler,
};
static pthread_t threadSig = NULL;
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

static struct signal *lookup_signal2(pthread_t tid, int signum)
{
	struct signal * sig;

	switch(signum)
	{
		case SIGKILL:
			sig = (struct signal*)&sig_default;
		break;
		default:
			sig = lookup_signal(tid, signum);
		break;
	}

	return sig;
}

static void sig_default_hadler(int signum)
{
	switch(signum)
	{
		case SIGKILL:
			pthread_exit(NULL);
		break;
		default:
			asAssert(0);
		break;
	}
}

void* signal_thread(void* arg)
{
	TaskType id;
	imask_t imask;
	const TaskConstType* pTaskConst;
	TaskVarType* pTaskVar;
	ASLOG(ON,"Signal Daemon Thread is running!\n");
	while(1)
	{
		/* yes, I know, this is a ugly implementation */
		Irq_Save(imask);
		for(id=0; id < OS_PTHREAD_NUM; id++)
		{
			pTaskVar   = &TaskVarArray[TASK_NUM+id];
			pTaskConst = pTaskVar->pConst;
			if((NULL != pTaskConst) && ((void*)1 != pTaskConst))
			{
				pthread_kill((pthread_t)pTaskConst, SIGALRM);
			}
		}
		Irq_Restore(imask);
		Os_Sleep(1);
	}

	return NULL;
}

static void signal_thread_autostart(int signum)
{
	pthread_attr_t attr;
	struct sched_param param;

	if(SIGALRM == signum)
	{
		if(NULL == threadSig)
		{
			pthread_attr_init(&attr);
			param.sched_priority = OS_PTHREAD_PRIORITY-1; /* highest */

			pthread_attr_setschedparam(&attr, &param);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			if(0 != pthread_create(&threadSig, &attr, signal_thread, NULL))
			{
				ASLOG(ERROR, "posix signal daemon thread create failed!\n");
			}
		}
	}
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
	threadSig = NULL;
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
ELF_EXPORT(sigaddset);

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
ELF_EXPORT(sigdelset);

int sigemptyset (sigset_t *set)
{
	*set = (sigset_t)0;
	return 0;
}
ELF_EXPORT(sigemptyset);

int sigfillset (sigset_t *set)
{
	*set = (sigset_t)-1;
	return 0;
}
ELF_EXPORT(sigfillset);

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
ELF_EXPORT(sigismember);

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
				/* call to start signal thread if necessary */
				signal_thread_autostart(signum);
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
ELF_EXPORT(sigaction);

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
ELF_EXPORT(signal);

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
		tid->sigWait = 0;
		Irq_Restore(imask);
	}
	else
	{
		ercd = -EINVAL;
	}

	return ercd;
}
ELF_EXPORT(sigwait);

int pthread_kill (pthread_t tid, int signum)
{
	int ercd = 0;
	struct signal * sig = NULL;
	imask_t imask;

	if(signum >= NSIG)
	{
		return -EINVAL;
	}

	asAssert(TCL_ISR2 != CallLevel);
	asAssert((tid->pTaskVar-TaskVarArray) >= TASK_NUM);
	asAssert((tid->pTaskVar-TaskVarArray) < (TASK_NUM+OS_PTHREAD_NUM));

	if(NULL != tid->pTaskVar->pConst)
	{
		Irq_Save(imask);
		sig = lookup_signal2(tid, signum);
		Irq_Restore(imask);
		if(NULL != sig)
		{
			if(tid->pTaskVar != RunningVar)
			{
				Irq_Save(imask);
				if(0 == Os_PortInstallSignal(tid->pTaskVar, signum, sig->action.sa_handler))
				{
					#ifdef USE_SCHED_LIST
					/* this is ugly signal call as treating signal call the highest priority */
					Sched_PosixAddReady(RunningVar - TaskVarArray);
					ReadyVar = tid->pTaskVar;
					Os_PortDispatch();
					#else
					/* kick the signal call immediately by an extra activation */
					Sched_PosixAddReady(tid->pTaskVar - TaskVarArray);
					#endif
				}
				else
				{
					ercd = -ENOMEM;
				}
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
ELF_EXPORT(pthread_kill);

int raise (int sig)
{
	return pthread_kill(pthread_self(), sig);
}
ELF_EXPORT(raise);
#endif /* USE_PTHREAD_SIGNAL */
#endif /* OS_PTHREAD_NUM */
