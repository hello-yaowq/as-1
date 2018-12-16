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
#include "Std_Types.h"
/* ============================ [ MACROS    ] ====================================================== */
#define TIMER_IRQ					(27)
#define CNTV_CTL_ENABLE		(1 << 0)	/* Enables the timer */
#define CNTV_CTL_IMASK		(1 << 1)	/* Timer interrupt mask bit */
#define CNTV_CTL_ISTATUS	(1 << 2)	/* The status of the timer interrupt. This bit is read-only */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void Irq_Install(int irqno, void (*handler)(void));
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
	Control register for the virtual timer.

	ISTATUS, bit [2]:	The status of the timer interrupt.
	IMASK, bit [1]:		Timer interrupt mask bit.
	ENABLE, bit [0]:	Enables the timer.
*/
static uint32_t raw_read_cntv_ctl(void)
{
	uint32_t cntv_ctl;

	__asm__ __volatile__("mrs %0, CNTV_CTL_EL0\n\t" : "=r" (cntv_ctl) :  : "memory");
	return cntv_ctl;
}

/* CNTVCT_EL0, Counter-timer Virtual Count register
	Holds the 64-bit virtual count value.
*/
static uint64_t raw_read_cntvct_el0(void)
{
	uint64_t cntvct_el0;

	__asm__ __volatile__("mrs %0, CNTVCT_EL0\n\t" : "=r" (cntvct_el0) : : "memory");
	return cntvct_el0;
}

static void raw_write_cntv_cval_el0(uint64_t cntv_cval_el0)
{
	__asm__ __volatile__("msr CNTV_CVAL_EL0, %0\n\t" : : "r" (cntv_cval_el0) : "memory");
}

/*
CNTFRQ_EL0, Counter-timer Frequency register
	Holds the clock frequency of the system counter.
*/
uint32_t raw_read_cntfrq_el0(void)
{
	uint32_t cntfrq_el0;

	__asm__ __volatile__("mrs %0, CNTFRQ_EL0\n\t" : "=r" (cntfrq_el0) : : "memory");
	return cntfrq_el0;
}

void raw_write_cntfrq_el0(uint32_t cntfrq_el0)
{
	__asm__ __volatile__("msr CNTFRQ_EL0, %0\n\t" : : "r" (cntfrq_el0) : "memory");
}
static void disable_cntv(void)
{
	uint32_t cntv_ctl;

	cntv_ctl = raw_read_cntv_ctl();
	cntv_ctl &= ~CNTV_CTL_ENABLE;
	__asm__ __volatile__("msr CNTV_CTL_EL0, %0\n\t" : : "r" (cntv_ctl) : "memory");
}

static void enable_cntv(void)
{
	uint32_t cntv_ctl;

	cntv_ctl = raw_read_cntv_ctl();
	cntv_ctl |= CNTV_CTL_ENABLE;
	__asm__ __volatile__("msr CNTV_CTL_EL0, %0\n\t" : : "r" (cntv_ctl) : "memory");
}

uint8 __weak SignalCounter(uint8 counter) { (void) counter; return 0; }

void __weak OsTick(void) { }

static void timer_isr_handler(void)
{
	uint64_t ticks,current_cnt;
	uint32_t cntfrq;

	OsTick();
	SignalCounter(0);

	cntfrq = raw_read_cntfrq_el0();
	ticks = cntfrq/OS_TICKS_PER_SECOND;
	current_cnt = raw_read_cntvct_el0();
	raw_write_cntv_cval_el0(current_cnt + ticks);
	enable_cntv();
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void Os_PortStartSysTick(void)
{
	uint64_t ticks,current_cnt;
	uint32_t cntfrq;

	Irq_Install(TIMER_IRQ, timer_isr_handler);

	cntfrq = raw_read_cntfrq_el0();
	ticks = cntfrq/OS_TICKS_PER_SECOND;
	current_cnt = raw_read_cntvct_el0();
	raw_write_cntv_cval_el0(current_cnt + ticks);
	enable_cntv();
}

void Os_PortStopSysTick(void)
{
	disable_cntv();
}
