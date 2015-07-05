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
#include "osek_kernel.h"
#include "task.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern void knl_activate_r(void);
/* ============================ [ DATAS     ] ====================================================== */
uint32 knl_taskindp;
uint32 knl_system_stack[SYSTEM_STACK_SIZE/sizeof(uint32)];
VP tcxb_sp[TASK_NUM];
FP tcxb_pc[TASK_NUM];
extern const FP tisr_pc[];
#if defined(CHIP_AT91SAM3S)
extern const uint32 __vector_table[];
#endif
uint32 knl_dispatch_started;
/* ============================ [ LOCALS    ] ====================================================== */
#if defined(CHIP_STM32F10X)
static void Usart_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
}

int putchar( int ch )	/* for printf */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
void set_ipl(IPL ipl)
{
	if(ipl > 0)
	{
		disable_int();
	}
}
IPL  current_ipl(void)
{
	return 0;
}
void activate_r(void)
{
    tcb_curpri[runtsk] = tinib_exepri[runtsk];
    callevel = TCL_PREPOST;
    PreTaskHook();
    callevel = TCL_TASK;
    enable_int();
    tinib_task[runtsk]();
}
void activate_context(TaskType TaskID)
{
    tcxb_pc[TaskID] = (FP)knl_activate_r;

	tcxb_sp[TaskID] = (VP)( (UINT32)tinib_stk[TaskID] + (UINT32)tinib_stksz[TaskID]);
}
void cpu_terminate(void)
{

}
void sys_exit(void)
{

}

void cpu_initialize(void)
{
#if defined(CHIP_AT91SAM3S)
	const uint32_t* pSrc;
	pSrc = __vector_table ;
	SCB->VTOR = ( (uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk ) ;

	if ( ((uint32_t)pSrc >= IRAM_ADDR) && ((uint32_t)pSrc < IRAM_ADDR+IRAM_SIZE) )
	{
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos ;
	}

	WDT_Disable(WDT);
	/* Low level Initialize */
	LowLevelInit() ;
#endif

#if defined(CHIP_STM32F10X)
	Usart_Init();
#endif
	knl_taskindp = 0;
	knl_dispatch_started = FALSE;
	if (SysTick_Config(64000000 / 1000))
	{
		/* Capture error */
		while (1);
	}

}

void knl_system_tick_handler(void)
{
	if(knl_dispatch_started == TRUE)
	{
		SignalCounter(0);
	}
#if defined(CHIP_AT91SAM3S)
	TimeTick_Increment();
#endif
}

void knl_isr_handler(uint32 intno)
{
	if( (intno>15) &&  (intno<51) && (tisr_pc[intno-16]!=NULL))
	{
		tisr_pc[intno-16]();
	}
	else
	{
		ShutdownOS(0xFF);
	}
}

void sys_initialize(void)
{

}
void tool_initialize(void)
{

}
