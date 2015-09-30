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
#ifndef COM_AS_INFRASTRUCTURE_INCLUDE_COMPILER_H_
#define COM_AS_INFRASTRUCTURE_INCLUDE_COMPILER_H_
/* ============================ [ INCLUDES  ] ====================================================== */

/* ============================ [ MACROS    ] ====================================================== */

/* REQ:COMPILER040,049,051 */
#define AUTOMATIC
#define STATIC 	static
#define NULL_PTR	((void *)0)

#if defined(__GNUC__)
#define CC_EXTENSION 	__extension__
#elif defined(__CWCC__)
#define CC_EXTENSION
#pragma read_only_switch_tables on
#elif defined(__DCC__)
#define CC_EXTENSION
#endif


#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})



#if defined(__GNUC__)
#define __balign(x)       __attribute__ ((aligned (x)))
#elif defined(__CWCC__)
#define __balign(x)       __attribute__ ((aligned (x)))
#elif defined(__DCC__)
#define __balign(x)       __attribute__ ((aligned (x)))
#elif defined(__ICCHCS12__)
#define Pragma(x) _Pragma(#x)
#define __balign(x)       Pragma(data_alignment=x)
#else
#error Compiler not defined.
#endif

#define SECTION_BALIGN(x)  __balign(x)

#if defined(__ICCHCS12__)
#define restrict
#define DECLARE_WEAK
#define __simple __simple
#else
#define DECLARE_WEAK			__attribute__ ((weak))
#define __simple
#endif

#ifndef __weak
# define __weak			__attribute__((weak))
#endif

#ifndef __packed
# define __packed		__attribute__((__packed__))
#endif

#define ilog2(_x)			__builtin_ffs(_x)

/* Does this really work on all compilers.... */
#define INLINE __inline__

/* REQ:COMPILER005 */
#define FUNC(rettype,memclass) rettype

#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *

#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *

#define CONSTP2VAR(ptrtype,memclass,ptrclass) ptrtype * const

#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const

#define P2FUNC(rettype,ptrclass,fctname) rettype (*fctname)

#ifndef CONST // for WIN32
#define CONST(consttype,memclass) const consttype
#endif

#define VAR(vartype,memclass) vartype
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */

#endif /* COM_AS_INFRASTRUCTURE_INCLUDE_COMPILER_H_ */
