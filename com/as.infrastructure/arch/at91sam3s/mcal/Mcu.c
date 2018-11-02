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
#include "board.h"

#ifdef USE_RINGBUFFER
#include "ringbuffer.h"
#endif
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void LowLevelInit( void );

#ifdef USE_USB_SERIAL
void USB_SerialPutChar(char ch);
#endif
/* ============================ [ DATAS     ] ====================================================== */
#ifdef USE_RINGBUFFER
RB_DECLARE(stdio, char, 512);
#endif
/* ============================ [ LOCALS    ] ====================================================== */
#ifdef USE_RINGBUFFER
static void flush_stdio(void)
{
	char ch0;
	rb_size_t r0;
	imask_t imask;

	Irq_Save(imask);
	r0 = RB_Pop(&rb_stdio, &ch0, 1);
	Irq_Restore(imask);

	if(1 == r0)
	{
		UART_PutChar(ch0);
	}
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
void __putchar(char ch)
{
#ifdef USE_RINGBUFFER
	rb_size_t r;
	imask_t imask;
	do {
		Irq_Save(imask);
		r = RB_Push(&rb_stdio, &ch, 1);
		Irq_Restore(imask);
		if(0 == r)
		{
			flush_stdio();
		}
	} while(r != 1);
#else
	UART_PutChar(ch);
#endif

#ifdef USE_USB_SERIAL
	USB_SerialPutChar(ch);
#endif
}

#ifndef __GNUC__
int putchar( int ch )	/* for printf */
{
	__putchar(ch);
  return ch;
}
#endif
void Mcu_Init(const Mcu_ConfigType *configPtr)
{
	(void)configPtr;
	WDT_Disable(WDT);
	/* Low level Initialize */
	LowLevelInit() ;
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
  return BOARD_MCK;
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
	UART_Configure(115200, BOARD_MCK);
}

void TaskIdleHook(void)
{
#ifdef USE_SHELL
	if (UART_IsRxReady())
	{
		char ch = UART_GetChar();
		if('\r' == ch)
		{
			ch = '\n';
		}
		SHELL_input(ch);
	}
#endif

#ifdef USE_RINGBUFFER
	if(0 != (UART0->UART_SR & UART_SR_TXEMPTY))
	{
		flush_stdio();
	}
#endif
}
