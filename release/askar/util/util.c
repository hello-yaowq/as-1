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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Os.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#ifdef __arch_versatilepb__
extern void serial_send_char(char ch);
void timer_init(void (*cbk)(void));
void timer_stop(void);
void vic_setup(void);
void irq_init(void);
#endif
extern ISR(ISR2);
extern ISR(ISR3);
/* ============================ [ DATAS     ] ====================================================== */
static volatile int isr2Flag;
static volatile int isr3Flag;
#ifdef __arch_versatilepb__
static int vicInitFlag = 0;
#endif
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void __weak ErrorHook(StatusType Error)
{
	
}

void isr2_handler(void)
{
	isr2Flag++;
	printf(" >> ISRMainISR2\n");
	ISRMainISR2();
}

void isr3_handler(void)
{
	isr3Flag++;
	printf(" >> ISRMainISR2\n");
	ISRMainISR3();
}

void __putchar(char ch)
{
#ifdef __arch_versatilepb__
	serial_send_char(ch);
#endif
}

void TriggerISR2(void)
{
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}
	isr2Flag = 0;
	timer_init(isr2_handler);
	while(isr2Flag == 0);
	timer_stop();
#endif
}

void TriggerISR3(void)
{
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}
	isr3Flag = 0;
	timer_init(isr3_handler);
	while(isr3Flag == 0);
	timer_stop();
#endif
}
