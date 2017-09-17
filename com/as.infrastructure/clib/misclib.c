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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static int _eval(const char **expr)
{
	int number=0;
	int result=0;
	char operator = '+';

	while(**expr != '\0')
	{
		if(**expr == '(')
		{
			(*expr)++;
			number = eval(expr);
		}
		else
		{
			while((**expr != ')') && (**expr != '+') && (**expr != '-') && (**expr !=  '*') && (**expr != '/') && (**expr != '\0'))
			{
				number = number*10 + (**expr) - '0';
				(*expr)++;
			}
		}

		switch(operator)
		{
			case '+':
				result=result+number;
				break;
			case '-':
				result=result-number;
				break;
			case '*':
				result=result*number;
				break;
			case '/':
				result=result/number;
				break;
		}

		if(**expr == ')')
		{
			(*expr) ++;
			break;
		}

		if(**expr != '\0')
		{
			operator=**expr;
			(*expr) ++;
		}

		number = 0;
	}

	return result;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
int eval(const char *expr)
{
	return _eval(&expr);
}
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

int strncmp(const char *s1, const char *s2, size_t n)
{
	for ( ; (*s1 == *s2) && (n > 0); s1++, s2++, n--)
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

/* ========================== [ cpoy from glib/gstrfuncs.c ] ===================== */
/**
 * strlcpy:
 * @dest: destination buffer
 * @src: source buffer
 * @dest_size: length of @dest in bytes
 *
 * Portability wrapper that calls strlcpy() on systems which have it,
 * and emulates strlcpy() otherwise. Copies @src to @dest; @dest is
 * guaranteed to be nul-terminated; @src must be nul-terminated;
 * @dest_size is the buffer size, not the number of bytes to copy.
 *
 * At most @dest_size - 1 characters will be copied. Always nul-terminates
 * (unless @dest_size is 0). This function does not allocate memory. Unlike
 * strncpy(), this function doesn't pad @dest (so it's often faster). It
 * returns the size of the attempted result, strlen (src), so if
 * @retval >= @dest_size, truncation occurred.
 *
 * Caveat: strlcpy() is supposedly more secure than strcpy() or strncpy(),
 * but if you really want to avoid screwups, g_strdup() is an even better
 * idea.
 *
 * Returns: length of @src
 */
size_t strlcpy (char       *dest,
           const char *src,
           size_t        dest_size)
{
	char *d = dest;
	const char *s = src;
	size_t n = dest_size;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) 
	{
		do {
			char c = *s++;

			*d++ = c;
			if (c == 0)
				break;
		} while (--n != 0);
	}

	/* If not enough room in dest, add NUL and traverse rest of src */
	if (n == 0)
    {
		if (dest_size != 0)
		{
			*d = 0;
		}
		while (*s++)
			;
    }

	return s - src - 1;  /* count does not include NUL */
}

/**
 * strlcat:
 * @dest: destination buffer, already containing one nul-terminated string
 * @src: source buffer
 * @dest_size: length of @dest buffer in bytes (not length of existing string
 *     inside @dest)
 *
 * Portability wrapper that calls strlcat() on systems which have it,
 * and emulates it otherwise. Appends nul-terminated @src string to @dest,
 * guaranteeing nul-termination for @dest. The total size of @dest won't
 * exceed @dest_size.
 *
 * At most @dest_size - 1 characters will be copied. Unlike strncat(),
 * @dest_size is the full size of dest, not the space left over. This
 * function does not allocate memory. It always nul-terminates (unless
 * @dest_size == 0 or there were no nul characters in the @dest_size
 * characters of dest to start with).
 *
 * Caveat: this is supposedly a more secure alternative to strcat() or
 * strncat(), but for real security g_strconcat() is harder to mess up.
 *
 * Returns: size of attempted result, which is MIN (dest_size, strlen
 *     (original dest)) + strlen (src), so if retval >= dest_size,
 *     truncation occurred.
 */
size_t strlcat (char       *dest,
           const char *src,
           size_t        dest_size)
{
	char *d = dest;
	const char *s = src;
	size_t bytes_left = dest_size;
	size_t dlength;  /* Logically, MIN (strlen (d), dest_size) */

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (*d != 0 && bytes_left-- != 0) 
	{
		d++;
	}
	dlength = d - dest;
	bytes_left = dest_size - dlength;

	if (bytes_left == 0)
	{
		return dlength + strlen (s);
	}
	while (*s != 0)
    {
		if (bytes_left != 1)
        {
			*d++ = *s;
			bytes_left--;
        }
		s++;
    }
	*d = 0;

	return dlength + (s - src);  /* count does not include NUL */
}
