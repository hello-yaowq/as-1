#include "SystemConfig.h"

#ifdef __arch_raspi__
#include "uart.h"
#include "regs.h"
#include "timer.h"
#include "interrupts.h"
#endif



OS_STK MainTask_Stk[MainTask_StkSize];
OS_STK App1Task_Stk[App1Task_StkSize];
OS_STK App2Task_Stk[App2Task_StkSize];
int main(void)
{
#ifdef __arch_raspi__
	uart_init();

	InitInterruptController();

	DisableInterrupts();

	timer_init();
#endif
	OSInit();

	OSTaskCreate(MainTask, (void *)0, &MainTask_Stk[MainTask_StkSize-1], MainTask_Prio);
	OSStart();
	return 0;
}

void MainTask(void *p_arg)
{
	p_arg = p_arg;
	OSStatInit();
	while(1)
	{

		OSTaskCreate(App1Task, (void *)0, &App1Task_Stk[App1Task_StkSize-1], App1Task_Prio);
		OSTaskCreate(App2Task, (void *)0, &App2Task_Stk[App2Task_StkSize-1], App2Task_Prio);
		printf("Hello,I am Main!\n");
		OSTimeDlyHMSM(0, 1, 0, 0);
		printf("Hello,Main End!\n");
        OSTaskDel(MainTask_Prio);
	}
}
/* App1Task */
void App1Task(void *p_arg)
{
	p_arg = p_arg;
	while(1)
	{
		printf("Hello,I am PC!\n");
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
/* App2Task */
void App2Task(void *p_arg)
{
	p_arg = p_arg;
	while(1)
	{
		printf("Hello,uCOS-II!\n");
		OSTimeDlyHMSM(0, 0, 2, 0);
	}
}

void OSDebugInit()
{
}
