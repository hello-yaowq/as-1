/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
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
#include "asdebug.h"
#include <stdarg.h>
#include <ctype.h>
#include <execinfo.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
typedef void (*aslog_t)(char*,char*);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
#if defined(__WINDOWS__) || defined(__LINUX__)
static char* __aswho  = "parai";
static aslog_t __aslog  = NULL;
static int g_argc=0;
static char** g_argv = 0;
#endif
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#if defined(__WINDOWS__) || defined(__LINUX__)
void aslog_init(char* who, aslog_t handler)
{
	__aswho = strdup(who);
	__aslog = handler;
}
char* aswho(void)
{
	return __aswho;
}
void aslog(char* module,char* format,...)
{
	static char buf[1024*2];
	static char name[256];
	va_list args;

	va_start(args , format);
	sprintf(name,"%s.%s",__aswho,module);
	vsprintf(buf,format,args);
	if(NULL != __aslog)
	{
		__aslog(name,buf);
	}
	else
	{
		printf("%-16s :: %s",name,buf);
	}

	va_end(args);
}

void asmem(void* address,size_t size)
{
	uint32 i,j;
	uint8 *src;
	src = (uint8*)address;
	printf("@%s:\n",__aswho);
	if(8 == sizeof(unsigned long))
	{
		printf("     address    :: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	}
	else
	{
		printf(" address :: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	}

	for(i=0; i<(size+15)/16; i++)
	{
		unsigned long a = (unsigned long)src+i*16;
		if( 8 == sizeof(unsigned long))
		{
			printf("%08X%08X ::",(uint32_t)(a>>32),(uint32_t)a);
		}
		else
		{
			printf("%08X ::",(uint32_t)a);
		}

		fflush(stdout);
		for(j=0;j<16;j++)
		{
			if((i*16+j)<size)
			{
				printf(" %02X",src[i*16+j]);
			}
			else
			{
				printf("   ");
			}
		}
		printf("\t");
		for(j=0;j<16;j++)
		{
			if(((i*16+j)<size) && isprint(src[i*16+j]))
			{
				printf("%c",src[i*16+j]);
			}
			else
			{
				printf(".");
			}
		}
		printf("\n");
	}

    fflush(stdout);
}

char* ashex(unsigned long a)
{
	char *buf = (char*)malloc(20);
	asAssert(buf);

	if( 8 == sizeof(unsigned long))
	{
		sprintf(buf,"%08X%08Xh",(uint32_t)(a>>32),(uint32_t)a);
	}
	else
	{
		sprintf(buf,"%08Xh",(uint32_t)a);
	}

	return buf;
}
#endif

void asAssertErrorHook(void)
{
#if defined(__WINDOWS__) || defined(__LINUX__)
	int blen,i;
	void* buffer[256];
	char** names;
	blen = backtrace(buffer,256);
	names = backtrace_symbols(buffer,blen);
	printf("call stack trace is:\n");
	for(i=blen-1;i >= 0;i--)
	{
		printf("  %3d: %32s\n",blen-1-i,names[i]);
	}
	exit(-1);
#else
	while(1);
#endif
}

#if defined(__LINUX__) || defined(__WINDOWS__)
void asPerfSet(asperf_t *m)
{
	gettimeofday(m,NULL);
}
void asPerfLog(asperf_t *m0,asperf_t *m1,char* infor)
{
	float rv = m1->tv_sec-m0->tv_sec;

	if(m1->tv_usec > m0->tv_usec)
	{
		rv += (float)(m1->tv_usec-m0->tv_usec)/1000000.0;
	}
	else
	{
		rv = rv - 1 + (float)(1000000.0+m1->tv_usec-m0->tv_usec)/1000000.0;
	}

	aslog("Perf","%s :: cost %f s\n",infor,rv);
}

void asEnvInit(int argc,char* argv[])
{
	g_argc = argc;
	g_argv = argv;
}
char* asEnvGet(int index)
{
	if(index < g_argc)
	{
		return g_argv[index];
	}
	else
	{
		asAssert(0);
	}

	return NULL;
}
#endif


