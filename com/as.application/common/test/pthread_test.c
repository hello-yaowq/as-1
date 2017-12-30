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
#include "Os.h"
#if(OS_PTHREAD_NUM > 0)
#include "pthread.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static pthread_t threadC;
static pthread_t threadP;
/* ============================ [ LOCALS    ] ====================================================== */
static void* consumer(void* arg)
{
	int count = 0;
	for(;;)
	{
		count ++;
		printf("consumer is running %d\n", count);
		Os_Sleep(300);
	}

	return NULL;
}

static void* producer(void* arg)
{
	int count = 0;
	for(;;)
	{
		count ++;
		printf("producer is running %d\n", count);
		Os_Sleep(200);
	}

	return NULL;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void pthread_test(void)
{
	int r;

	r = pthread_create(&threadC, NULL, consumer, NULL);

	if(0 != r)
	{
		printf("create pthread consumer failed!(%d)\n", r);
	}

	r = pthread_create(&threadP, NULL, producer, NULL);

	if(0 != r)
	{
		printf("create pthread producer failed!(%d)\n", r);
	}
}
#endif
