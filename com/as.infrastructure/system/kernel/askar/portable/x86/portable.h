/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
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
#ifndef PORTABLE_H_
#define PORTABLE_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "mmu.h"
#include "x86.h"
/* ============================ [ MACROS    ] ====================================================== */
#define LDT_SIZE		2
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	cpu_context_t regs; /* process' registers saved in stack frame */

	uint16_t ldt_sel;   /* selector in gdt giving ldt base and limit*/
	mmu_descriptor_t ldts[LDT_SIZE]; /* local descriptors for code and data */
									 /* 2 is LDT_SIZE - avoid include protect.h */

	void (*pc)(void);
} TaskContextType;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* PORTABLE_H_ */
