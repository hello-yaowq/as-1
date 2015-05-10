#include <stdio.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Lcd.h"
#include "Sg.h"

void Task1( void * param)
{
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
	Sg_Init();
	while(1)
	{
		Sg_ManagerTask();
		vTaskDelay(5);
	}
}
void Task2( void * param)
{
	while(1)
	{
		printf("Task2 is running!\n");
		vTaskDelay(1000);
	}
}
int main( int argc, char* argv[] )
{
	/* Start the tasks and timer running. */
	xTaskCreate( Task1, "Tasl1", 1024, NULL, 1, NULL );
	xTaskCreate( Task2, "Tasl2", 1024, NULL, 1, NULL );
	vTaskStartScheduler();
	return 0;
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
	_assert(__FUNCTION__, pcFileName, ulLine);
}

unsigned long ulGetRunTimeCounterValue( void )
{
	return 0;
}

void vConfigureTimerForRunTimeStats( void )
{

}
