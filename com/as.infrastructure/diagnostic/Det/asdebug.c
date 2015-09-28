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
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
typedef void (*aslog_t)(char*,char*);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static char* __aswho  = "parai";
static aslog_t __aslog  = NULL;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void aslog_init(char* who, aslog_t handler)
{
	__aswho = strdup(who);
	__aslog = handler;
}
char* aswho(void)
{
	return __aswho;
}
void aslog(char* format,...)
{
	static char* buf = NULL;
	va_list args;

	va_start(args , format);
	if(NULL == buf)
	{
		buf = malloc(1024);
		assert(buf);
	}
	vsprintf(buf,format,args);
	if(NULL != __aslog)
	{
		__aslog(__aswho,buf);
	}
	else
	{
		puts(buf);
	}

	va_end(args);
}
