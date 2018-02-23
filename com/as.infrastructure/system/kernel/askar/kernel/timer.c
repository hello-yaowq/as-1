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
#include <pthread.h>
#include "kernel_internal.h"
#if(OS_PTHREAD_NUM > 0)
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int getitimer (int which, struct itimerval *old)
{
	return 0;
}
ELF_EXPORT(getitimer);

int setitimer (int which, const struct itimerval *new, struct itimerval *old)
{
	return 0;
}
ELF_EXPORT(setitimer);

clock_t times (struct tms *buffer)
{
	buffer->tms_stime = OsTickCounter;
	buffer->tms_cstime = OsTickCounter;
	buffer->tms_stime = OsTickCounter;
	buffer->tms_utime = OsTickCounter;

	return OsTickCounter;
}
ELF_EXPORT(times);
#endif /* OS_PTHREAD_NUM */
