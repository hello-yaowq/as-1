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
#include <unistd.h>
/* ============================ [ MACROS    ] ====================================================== */
#define BUFFER_SIZE 16
#define OVER (-1)
#if BUFFER_SIZE < 2
#error BUFFER_SIZE must bigger than 2
#endif
/* ============================ [ TYPES     ] ====================================================== */
/* Circular buffer of integers. */
struct prodcons {
  int buffer[BUFFER_SIZE];      /* the actual data */
  pthread_mutex_t lock;         /* mutex ensuring exclusive access to buffer */
  int readpos, writepos;        /* positions for reading and writing */
  pthread_cond_t notempty;      /* signaled when buffer is not empty */
  pthread_cond_t notfull;       /* signaled when buffer is not full */
};
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static pthread_t threadC;
static pthread_t threadP;
struct prodcons buffer;
static uint32_t threadCStack[1024];
/* ============================ [ LOCALS    ] ====================================================== */
/* Initialize a buffer */

static void init(struct prodcons * b)
{
	pthread_mutex_init(&b->lock, NULL);
	pthread_cond_init(&b->notempty, NULL);
	pthread_cond_init(&b->notfull, NULL);
	b->readpos = 0;
	b->writepos = 0;
}

/* Store an integer in the buffer */
static void put(struct prodcons * b, int data)
{
	pthread_mutex_lock(&b->lock);
	/* Wait until buffer is not full */
	while ((b->writepos + 1) % BUFFER_SIZE == b->readpos) {
		pthread_cond_wait(&b->notfull, &b->lock);
		/* pthread_cond_wait reacquired b->lock before returning */
	}
	/* Write the data and advance write pointer */
	b->buffer[b->writepos] = data;
	b->writepos++;
	if (b->writepos >= BUFFER_SIZE) b->writepos = 0;
	/* Signal that the buffer is now not empty */
	pthread_cond_signal(&b->notempty);
	pthread_mutex_unlock(&b->lock);
}

/* Read and remove an integer from the buffer */

static int get(struct prodcons * b)
{
	int data;
	int r;
	struct timespec abstime = { 1, 0 };
	pthread_mutex_lock(&b->lock);
	/* Wait until buffer is not empty */
	while (b->writepos == b->readpos) {
		r = pthread_cond_timedwait(&b->notempty, &b->lock, &abstime);
		if(-ETIMEDOUT == r)
		{
			printf("consumer get timeout!\n");
		}
	}
	/* Read the data and advance read pointer */
	data = b->buffer[b->readpos];
	b->readpos++;
	if (b->readpos >= BUFFER_SIZE) b->readpos = 0;
	/* Signal that the buffer is now not full */
	pthread_cond_signal(&b->notfull);
	pthread_mutex_unlock(&b->lock);
	return data;
}
static void* consumer(void* arg)
{
	int d;
	int n;
	while (1) {
		d = get(&buffer);
		if (d == OVER) break;
		printf("---> %d\n", d);
	}
	for(n=0;n<100;n++)
	{
		sleep(2);
		printf("consumer is running %d\n",n);
	}
	return NULL;
}

static void* producer(void* arg)
{
	int n;
	for (n = 0; n < 10000; n++) {
		printf("%d --->\n", n);
		put(&buffer, n);
		//usleep(1000);
	}
	put(&buffer, OVER);
	for(n=0;n<100;n++)
	{
		sleep(1);
		printf("producer is running %d\n",n);
	}

	return NULL;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void pthread_test(void)
{
	int r;
	pthread_attr_t attr;
	struct sched_param param;
	init(&buffer);

	pthread_attr_init(&attr);
	param.sched_priority = OS_PTHREAD_PRIORITY-1; /* highest */

	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setstack(&attr, threadCStack, sizeof(threadCStack));

	r = pthread_create(&threadC, &attr, consumer, NULL);

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
