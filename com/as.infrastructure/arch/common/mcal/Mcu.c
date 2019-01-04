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
/* ============================ [ INCLUDES  ] ====================================================== */

#include <assert.h>
#include <string.h>
#include "Std_Types.h"
#include "Mcu.h"
#if defined(USE_DET)
#include "Det.h"
#endif
#if defined(USE_DEM)
#include "Dem.h"
#endif
#include "Os.h"
#ifdef USE_SHELL
#include "shell.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void __weak uart_putc(unsigned char byte)
{
	(void)byte;
}

int __weak uart_getc(void) { return -1; }

void __putchar(char ch)
{
	uart_putc(ch);
}

#ifndef __GNUC__
int putchar( int ch )	/* for printf */
{
	__putchar(ch);
  return ch;
}
#endif

void __weak Irq_Init(void) {}
void Mcu_Init(const Mcu_ConfigType *configPtr)
{
	(void)configPtr;
}

void Mcu_SetMode( Mcu_ModeType McuMode ) {
	(void)McuMode;
}

/**
 * Get the system clock in Hz. It calculates the clock from the
 * different register settings in HW.
 */
uint32_t McuE_GetSystemClock(void)
{
  return 0u;
}
Std_ReturnType Mcu_InitClock( const Mcu_ClockType ClockSetting ) {
	(void)ClockSetting;

    return E_OK;
}

void Mcu_PerformReset( void ) {

}

Mcu_PllStatusType Mcu_GetPllStatus( void ) {
	return MCU_PLL_LOCKED;
}


Mcu_ResetType Mcu_GetResetReason( void )
{
	return MCU_POWER_ON_RESET;
}

void Mcu_DistributePllClock( void )
{
	Irq_Init();
}
#ifdef USE_SHELL
void TaskIdleHook(void)
{
	int ch = uart_getc();
	if(-1 != ch)
	{
		SHELL_input(ch);
	}
}
#endif

int __weak rand (void)
{
	/* intentional not initialized to use the stack random value */
	int rv;
	uint32 seed;
	uint32 u32Time = GetOsTick();

	rv = rv ^ seed ^ u32Time ^ 0xfeedbeef;

	return rv;
}
