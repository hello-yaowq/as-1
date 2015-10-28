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
#if (ISR_NUM > 0)
extern const FP tisr_pc[ISR_NUM];
#endif
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
#endif

#if defined(CHIP_LM3S6965)
#include "hw_memmap.h"
#include "hw_types.h"
#include "gpio.h"
#include "lm3sinterrupt.h"
#include "sysctl.h"
#include "uart.h"
static void Usart_Init(void)
{
    /* Set the clocking to run directly from the crystal. */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    /* Enable the peripherals used by this example. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Enable processor interrupts. */
    /* IntMasterEnable(); */

    /* Set GPIO A0 and A1 as UART pins. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Configure the UART for 115,200, 8-N-1 operation. */
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    /* Enable the UART interrupt.
     *
     * IntEnable(INT_UART0);
     * UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
     */

    printf("Hello world from lm3s\n");
}
#endif

#ifdef __GNUC__
#else
int putchar( int ch )	/* for printf */
{
#if defined(CHIP_STM32F10X)
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
#elif defined(CHIP_LM3S6965)
  UARTCharPut(UART0_BASE, ch);
#endif
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

#if defined(CHIP_LM3S6965)
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
#if (ISR_NUM > 0)
	if( (intno>15) &&  (intno<(16+ISR_NUM)) && (tisr_pc[intno-16]!=NULL))
	{
		tisr_pc[intno-16]();
	}
	else
#endif
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
#ifdef __GNUC__
void __naked enaint( imask_t intsts )
{
__asm__ volatile(
    "mrs     r1, primask	\n"
    "msr     primask, r0	\n"
    "mov     r0, r1			\n"
    "bx      lr				\n"
);
}

imask_t __naked disint( void )
{
__asm__ volatile(
    "mrs     r0, primask	\n"
    "ldr     r1, =0x01  	\n"	/* =TS_PMK_D*/
    "msr     primask, r1	\n"
    "bx      lr				\n"
);
}

void __naked knl_activate_r(void)
{
__asm__ volatile(
	"mov 	r3, #0x01000000	\n" /* TS_PSR_T */
	"ldr 	r2, =activate_r	\n"
	"push 	{r2,r3}			\n"
	"subs  	SP,SP,#24		\n"
	"bx 	lr				\n"
);
}

void __naked knl_dispatch_r(void)
{
__asm__ volatile(
     /* start to restore task's context */
    "pop     {r4-r11}	\n"
    "cpsie   i			\n"
    "bx      lr			\n"
);
}

void __naked dispatch_task(void)
{
__asm__ volatile(
    "ldr     r2, =tcxb_pc		\n"
    "ldr     r3,[r2,r0,LSL #2]	\n"
    "bx      r3					\n"
);
}
#define knl_dispatch_ret_int knl_start_dispatch
void __naked knl_start_dispatch(void)
{
__asm__ volatile(
    "ldr     r0, =schedtsk			\n"
    "ldrb    r0, [r0]				\n"
    "ldr     r1, =runtsk			\n"
    "strb    r0, [r1]				\n"

    "ldr     r2, =tcxb_sp			\n"
    "ldr     sp, [r2,r0, LSL #2]	\n"
    "b       dispatch_task			\n"
);
}

void __naked knl_dispatch_entry(void)
{
__asm__ volatile(
    "push    {r4-r11}				\n"

    "ldr     r1, =runtsk			\n"
    "ldrb    r0, [r1]				\n"

    "ldr     r2, =tcxb_sp			\n"
    "str     sp, [r2,r0, LSL #2]	\n"

    "ldr     r2, =tcxb_pc			\n"

    "ldr     r12, =knl_dispatch_r	\n"
    "str     r12, [r2,r0, LSL #2]	\n"

    "b       knl_start_dispatch		\n"
);
}

void __naked EnterISR(void)
{
__asm__ volatile(
	"ldr     r1, =knl_taskindp		\n"
    "ldr     r2, [r1]				\n"

    "cmp     r2, #0					\n"      /* knl_taskindp==0 */
    "bne     l_nosave				\n"

    "ldr     r3, = knl_dispatch_started	\n"
    "ldr     r3, [r3]				\n"
    "cmp     r3, #0					\n"
    "beq     l_nosave				\n"    /* system not startd */

    /* save context on fisrt ISR enter */
    "push    {r4-r11}				\n"

    "ldr     r3, =runtsk			\n"
    "ldrb    r4, [r3]				\n"

    "ldr     r3, =tcxb_sp			\n"
    "str     sp, [r3,r4, LSL #2]	\n"

    "ldr     r3, =tcxb_pc			\n"

    "ldr     r12, =knl_dispatch_r	\n"
    "str     r12, [r3,r4, LSL #2]	\n"
    // and then load isr system stack
    "ldr     sp, =(knl_system_stack + 1024)	\n"  /* Set system stack SYSTEM_STACK_SIZE*/

"l_nosave: 							\n"
    "push    {r0}					\n"    /* push {lr} */
    "add     r3, r2, #1				\n"
    "str     r3, [r1]				\n"
    "push    {r1, r2}				\n"
    "ldr     r1, = callevel			\n"
    "ldrb    r3, [r1]				\n"
    "push    {r3}					\n"
    "mov     r3, #2					\n"    /* callevel = TCL_ISR2 */
    "strb    r3,[r1]				\n"
    "cpsie   i						\n"
    "bx      lr						\n"
);
}


void __naked ExitISR(void)
{
__asm__ volatile(
	"pop     {r3}						\n"
	"ldr     r1, = callevel				\n"
	"strb    r3, [r1]					\n"
	"pop     {r1,r2}					\n"
	"str     r2, [r1]					\n"
	"pop     {lr}						\n"
	"cpsid   i							\n"

	"ldr     r0, =knl_taskindp			\n"
	"ldr     r0, [r0]					\n"
	"cmp     r0, #0						\n"
	"bne     l_nodispatch				\n"

	"ldr     r0, = knl_dispatch_started	\n"
	"ldr     r0, [r0]					\n"
	"cmp     r0, #0						\n"
	"beq     l_nodispatch				\n"

	"b      knl_dispatch_entry 			\n"  /* To dispatch processing  knl_dispatch_ret_int*/

"l_nodispatch:							\n"
	"cpsie   i							\n"
	"bx      lr							\n"
);
}

void __naked knl_system_tick(void)
{
__asm__ volatile(
    "mov r0,lr						\n"
    "bl EnterISR					\n"
    "bl knl_system_tick_handler		\n"
    "b  ExitISR						\n"
);
}

void __naked knl_isr_process(void)
{
__asm__ volatile(
    "mov r0,lr					\n"
    "bl EnterISR				\n"
    "mrs     r0, ipsr 			\n"   /* r0 = dintno */
    "bl knl_isr_handler			\n"
    "b  ExitISR					\n"
);
}
#endif /* __GNUC__ */

void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function)
{
	printf("assert(%s) @ %s line %d of %s\n",__assertion,__file,__line,__function);
	while(1);
}
