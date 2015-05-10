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
#ifdef CHIP_AT91SAM3S
#include "board.h"
#else
#include "Lcd.h"
#endif
#include "Sg.h"
/* ============================ [ MACROS    ] ====================================================== */
/** Test image height. */
#define IMAGE_HEIGHT    50
/** Test image width. */
#define IMAGE_WIDTH     100
/** Number of color block in horizontal direction of test image */
#define N_BLK_HOR       5
/** Number of color block in vertical direction of test image */
#define N_BLK_VERT      4

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static uint8 ledCounter;

/** Image buffer. */
static uint8_t gImageBuffer[IMAGE_HEIGHT * IMAGE_WIDTH * 3];

/** Color pattern for make image. */
static const uint32_t gColorPattern[N_BLK_HOR*N_BLK_VERT] = {
    COLOR_BLACK,    COLOR_YELLOW, COLOR_RED,         COLOR_GREEN,  COLOR_BLUE,
    COLOR_CYAN,     COLOR_WHITE,  COLOR_INDIGO,      COLOR_OLIVE,  COLOR_BROWN,
    COLOR_GRAY,     COLOR_SIENNA, COLOR_GREENYELLOW, COLOR_SILVER, COLOR_VIOLET,
    COLOR_DARKBLUE, COLOR_ORANGE, COLOR_DARKGREEN,   COLOR_TOMATO, COLOR_GOLD,
};
/**
 * \brief Make a test Image with special pattern.
 *
 * \param pBuffer Image buffer.
 *
 * \note The image is a raw data of RGB 24-bit format.
 */
static void MakeTestImage( uint8_t *pBuffer )
{
    uint32_t v_max  = IMAGE_HEIGHT;
    uint32_t h_max  = IMAGE_WIDTH;
    uint32_t v_step = (v_max + N_BLK_VERT - 1) / N_BLK_VERT;
    uint32_t h_step = (h_max + N_BLK_HOR  - 1) / N_BLK_HOR;
    uint32_t v, h;
    uint8_t  *pImage = (uint8_t *)pBuffer;
    uint8_t ix ;
    uint8_t iy ;

    for ( v = 0; v < v_max; v++ )
    {
        iy = v / v_step ;

        for ( h = 0 ; h < h_max ; h++ )
        {
            ix = N_BLK_HOR * iy + (h / h_step);
            *pImage++ = (gColorPattern[ix] & 0x0000FF);
            *pImage++ = (gColorPattern[ix] & 0x00FF00) >> 8;
            *pImage++ = (gColorPattern[ix] & 0xFF0000) >> 16;
        }
    }
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void StartupHook(void)
{
	ledCounter = 0;
#ifdef CHIP_AT91SAM3S
	LCDD_Initialize();
	LCDD_On();
	LED_Configure(LED_RED);
	LED_Configure(LED_BLUE);
	LED_Configure(LED_GREEN);

	LCDD_DrawString( 30, 20, (uint8_t *)"smc_lcd example", COLOR_BLACK ) ;

	MakeTestImage(gImageBuffer);
	LCDD_DrawImage(60, 60, (const uint8_t *)gImageBuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

	LCDD_DrawCircle(60,  160, 40, COLOR_RED);
	LCDD_DrawCircle(120, 160, 40, COLOR_GREEN);
	LCDD_DrawCircle(180, 160, 40, COLOR_BLUE);

	LCDD_DrawRectangle(20, 220, 200, 80, COLOR_VIOLET);
	LCDD_DrawLine(10,  260, 220, DIRECTION_HLINE, COLOR_CYAN);
	LCDD_DrawLine(120, 210, 100, DIRECTION_VLINE, COLOR_ORANGE);
#else
	Lcd_Init(SG_LCD_WIGTH,SG_LCD_HEIGHT,1);
	Sg_Init();
#endif
}

TASK(TaskApp)
{
	LED_Clear(LED_BLUE);
	LED_Clear(LED_GREEN);
	ledCounter ++;
	if(50 <= ledCounter)	/* 50x20ms = 1000ms */
	{
		LED_Toggle(LED_RED);
		LCD_SetDisplayLandscape(0x112233);
		ledCounter = 0;
	}

	OsTerminateTask(TaskApp);
}
TASK(TaskCom)
{
	OsTerminateTask(TaskCom);
}
TASK(TaskSg)
{
#ifdef CHIP_AT91SAM3S
#else  
	Sg_ManagerTask();
#endif
	OsTerminateTask(TaskSg);
}

TASK(TaskIdle)
{
	for(;;);
}
ALARM(Alarm5ms)
{
	OsActivateTask(TaskSg);
	OsActivateTask(TaskCom);
}
ALARM(Alarm10ms)
{
}
ALARM(Alarm20ms)
{
	OsActivateTask(TaskApp);
}
ALARM(Alarm50ms)
{
}
ALARM(Alarm100ms)
{
}

void ErrorHook(StatusType ercd)
{

	switch(ercd)
	{
		case E_OS_ACCESS:
			printf("ercd = %d E_OS_ACCESS!\r\n",ercd);
			break;
		case E_OS_CALLEVEL:
			printf("ercd = %d E_OS_CALLEVEL!\r\n",ercd);
			break;
		case E_OS_ID:
			printf("ercd = %d E_OS_ID!\r\n",ercd);
			break;
		case E_OS_LIMIT:
			printf("ercd = %d E_OS_LIMIT!\r\n",ercd);
			break;
		case E_OS_NOFUNC:
			printf("ercd = %d E_OS_NOFUNC!\r\n",ercd);
			break;
		case E_OS_RESOURCE:
			printf("ercd = %d E_OS_RESOURCE!\r\n",ercd);
			break;
		case E_OS_STATE:
			printf("ercd = %d E_OS_STATE!\r\n",ercd);
			break;
		case E_OS_VALUE	:
			printf("ercd = %d E_OS_VALUE!\r\n",ercd);
			break;
		default:
			printf("ercd = %d unknown error!\r\n",ercd);
			break;
	}

	if(E_OS_CALLEVEL == ercd)
	{
		ShutdownOS(E_OS_CALLEVEL);
	}
	else
	{
		/* recover-able error */
	}
}
void PreTaskHook(void)
{

}
void PostTaskHook(void)
{

}
void ShutdownHook(StatusType ercd)
{
	for(;;);
}
