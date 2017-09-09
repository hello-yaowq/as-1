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

#include "Std_Types.h"
#include "Mcu.h"
#include "Os.h"
#include "asdebug.h"
#ifdef __RTTHREAD_OS__
#include <rtthread.h>
#include <rthw.h>
#endif
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_MCU 1
#define enable_int() asm("sti")
#define disable_int() asm("cli")

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static unsigned long isrDisableCounter = 0;

/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void __putchar(char ch)
{
	(void)ch;
#ifdef __RTTHREAD_OS__
	rt_console_putc(ch);
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

#ifdef __AS_BOOTLOADER__
void StartOsTick(void)
{

}
#endif

void tpl_shutdown(void)
{
	while(1);
}
#ifdef __RTTHREAD_OS__
void rt_low_level_init(void)
{
}
void tpl_primary_syscall_handler(void)
{
	while(1);
}
#endif
void Mcu_DistributePllClock( void )
{

}

int EnableInterrupts() {
	enable_int();
	return 0;
}

int DisableInterrupts() {
	disable_int();
	return 0;
}

imask_t __Irq_Save(void)
{
	isrDisableCounter ++ ;
	if(1u == isrDisableCounter)
	{
		disable_int();
	}
	return 0;
}

void Irq_Restore(imask_t irq_state)
{

	isrDisableCounter --;
	if(0u == isrDisableCounter)
	{
		enable_int();
	}

	(void)irq_state;
}

void  Irq_Enable(void)
{
	enable_int();
}

#ifdef __RTTHREAD_OS__
extern unsigned char __bss_start[];
extern unsigned char __bss_end[];
extern void main(void);
extern void rt_hw_console_init(void);

void rt_hw_clear_bss(void)
{
    unsigned char *dst;
    dst = __bss_start;
    while (dst < __bss_end)
        *dst++ = 0;
}

void rtthread_startup(void)
{
    rt_hw_clear_bss();

    rt_hw_interrupt_init();

    rt_hw_console_init();
    rt_console_set_device("console");

    rt_hw_board_init();

    rt_show_version();

	main();
}
#endif /* __RTTHREAD_OS__ */

