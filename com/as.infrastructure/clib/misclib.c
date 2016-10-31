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
#include <Std_Types.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void *memset (void *__s, int __c, size_t __n)
{
	size_t i;
	char* ptr = (char*) __s;

	for(i=0;i<__n;i++)
	{
		ptr[i] = (char)(__c&0xFFu);
	}

	return __s;
}

void *memcpy (void* __to, const void* __from, size_t __size)
{
	size_t i;
	char* dst = (char*) __to;
	const char* src = (const char*) __from;

	for(i=0;i<__size;i++)
	{
		dst[i] = src[i];
	}

	return __to;
}

char* strcpy (char* __to, const char* __from)
{
	char* dst = (char*) __to;
	const char* src = (const char*) __from;
	while('\0' != *dst)
	{
		*dst = * src;
		dst ++;
		src ++;
	}

	*dst = '\0';
	return __to;
}

int
strcmp(const char *s1, const char *s2)
{
	for ( ; *s1 == *s2; s1++, s2++)
	if (*s1 == '\0')
	{
		return 0;
	}
	return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}


int
memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char u1, u2;

	for ( ; n-- ; s1++, s2++) {
		u1 = * (unsigned char *) s1;
		u2 = * (unsigned char *) s2;
		if ( u1 != u2) {
			return (u1-u2);
		}
	}
	return 0;
}
