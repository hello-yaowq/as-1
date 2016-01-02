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
#include "Os.h"
#if defined(__SMALL_OS__)
#include <sys/time.h>
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void Can_SimulatorRunning(void);
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
#if defined(__SMALL_OS__)
static clock_t previous = 0;
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
KSM(Simulator,Init)
{
	KGS(Simulator,Running);
}
KSM(Simulator,Start)
{

}
KSM(Simulator,Stop)
{

}
KSM(Simulator,Running)
{
	Can_SimulatorRunning();

#if defined(__SMALL_OS__)
	if(clock() != previous)
	{
		previous = clock();
		OsTick();
	}
#endif
}

#ifdef __AS_BOOTLOADER__
void application_main(void) { }
#endif


