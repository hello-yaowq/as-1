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
#ifndef COM_AS_INFRASTRUCTURE_INCLUDE_ASDEBUG_H_
#define COM_AS_INFRASTRUCTURE_INCLUDE_ASDEBUG_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Std_Types.h"
/* ============================ [ MACROS    ] ====================================================== */
/* levels for log output */
#define AS_LOG_RPROC    1
#define AS_LOG_CAN      2
#define AS_LOG_CANIF    3
/* and so on ... */
#define AS_LOG_DEFAULT  1
#if defined(__WINDOWS__) || defined(__LINUX__)
#define ASLOG(level,fmt,...) 					\
	if((level) >= AS_LOG_DEFAULT) {			\
		aslog(fmt,##__VA_ARGS__);					\
	}
#else
#define ASLOG(level,fmt,...) 			\
	if((level) >= AS_DFT_LOG) {			\
		printf(fmt,##__VA_ARGS__);		\
	}
#endif

#define PRINTF(fmt,...) ASLOG(AS_LOG_DEFAULT,fmt,##__VA_ARGS__)
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
extern void aslog(char* format,...);
char* aswho(void);

#endif /* COM_AS_INFRASTRUCTURE_INCLUDE_ASDEBUG_H_ */
