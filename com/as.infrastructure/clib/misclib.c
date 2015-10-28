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
static struct _reent impure_data = { 0, 0, "", 0, "C"};
struct _reent * _impure_ptr = &impure_data;
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

/*
 * Write the given character into the (probably full) buffer for
 * the given file.  Flush the buffer out if it is or becomes full,
 * or if c=='\n' and the file is line buffered.
 */

int __swbuf_r (struct _reent *ptr, register int c, register FILE *fp)
{
	/*
	 char i;

	i = (char)c;
	_write (fp->_file, &i, 1);
	*/

	putchar(c);

	return 0;
}
