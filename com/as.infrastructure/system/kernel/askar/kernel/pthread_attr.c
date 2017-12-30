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
#include <string.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
const pthread_attr_t pthread_default_attr =
{
	0,                          /* stack base */
	PTHREAD_DEFAULT_STACK_SIZE, /* stack size */
	PTHREAD_DEFAULT_PRIORITY,   /* priority */
	PTHREAD_CREATE_JOINABLE,    /* detach state */
	SCHED_FIFO,                 /* scheduler policy */
	PTHREAD_INHERIT_SCHED       /* Inherit parent prio/policy */
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int pthread_attr_init(pthread_attr_t *attr)
{

	*attr = pthread_default_attr;

	return 0;
}


int pthread_attr_destroy(pthread_attr_t *attr)
{
	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int state)
{
	int ercd = 0;

	if ((state != PTHREAD_CREATE_JOINABLE) && (state != PTHREAD_CREATE_DETACHED))
	{
		ercd = -EINVAL;
	}
	else
	{
		attr->detachstate = state;
	}

	return ercd;
}


int pthread_attr_getdetachstate(pthread_attr_t const *attr, int *state)
{
	*state = (int)attr->detachstate;

	return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	attr->policy = policy;

	return 0;
}

int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy)
{
	*policy = (int)attr->policy;

	return 0;
}

int pthread_attr_setschedparam(pthread_attr_t           *attr,
                               struct sched_param const *param)
{
	int ercd = 0;
	if(param->sched_priority < OS_PTHREAD_PRIORITY)
	{
		attr->priority = param->sched_priority;
	}
	else
	{
		ercd = -EINVAL;
	}

	return ercd;
}

int pthread_attr_getschedparam(pthread_attr_t const *attr,
                               struct sched_param   *param)
{
	param->sched_priority = attr->priority;

	return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size)
{

	attr->stack_size = stack_size;

	return 0;
}

int pthread_attr_getstacksize(pthread_attr_t const *attr, size_t *stack_size)
{
	*stack_size = attr->stack_size;

	return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stack_addr)
{
	return -EOPNOTSUPP;
}

int pthread_attr_getstackaddr(pthread_attr_t const *attr, void **stack_addr)
{
	return -EOPNOTSUPP;
}

int pthread_attr_setstack(pthread_attr_t *attr,
                          void           *stack_base,
                          size_t          stack_size)
{
	attr->stack_base = stack_base;
	attr->stack_size = stack_size;

	return 0;
}

int pthread_attr_getstack(pthread_attr_t const *attr,
                          void                **stack_base,
                          size_t               *stack_size)
{

	*stack_base = attr->stack_base;
	*stack_size = attr->stack_size;

	return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guard_size)
{
	return -EOPNOTSUPP;
}

int pthread_attr_getguardsize(pthread_attr_t const *attr, size_t *guard_size)
{
	return -EOPNOTSUPP;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
	int ercd = -EINVAL;

	if (scope == PTHREAD_SCOPE_SYSTEM)
	{
		/* pass */
	}
	else if (scope == PTHREAD_SCOPE_PROCESS)
	{
		ercd = -EOPNOTSUPP;
	}

	return ercd;
}

int pthread_attr_getscope(pthread_attr_t const *attr)
{
	return PTHREAD_SCOPE_SYSTEM;
}

#endif
