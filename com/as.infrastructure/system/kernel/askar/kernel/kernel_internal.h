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
#ifndef KERNEL_INTERNAL_H_
#define KERNEL_INTERNAL_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Os.h"
#include "portable.h"
/* ============================ [ MACROS    ] ====================================================== */
#define INVALID_TASK            ((TaskType)-1)
/* ============================ [ TYPES     ] ====================================================== */
typedef unsigned int			KeyType;
typedef KeyType					PriorityType;

typedef void	(*TaskMainEntryType)(void);
typedef void	(*AlarmMainEntryType)(void);

typedef struct
{
	void* pStack;
	uint32_t stackSize;
	TaskMainEntryType entry;
} TaskConstType;

typedef struct
{
	TaskContextType context;
} TaskVarType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* KERNEL_INTERNAL_H_ */
