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
#ifdef USE_LCD
/* ============================ [ INCLUDES  ] ====================================================== */
#include <Sg.h>
#include "Lcd.h"

#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define ARM_PL110_BASE 0x10120000
/* ============================ [ TYPES     ] ====================================================== */
typedef enum {
	PL110_240x320_25MHZ,
	PL110_320x240_25MHZ,
	PL110_176x220_25MHZ,
	PL110_640x480_25MHZ,
	PL110_800x600_36MHZ,
	PL110_176x220_10MHZ,
	PL110_320x240_10MHZ
} pl110_DisplayResolution;

typedef struct {
	uint32_t timing_0;
	uint32_t timing_1;
	uint32_t timing_2;
	uint32_t timing_3;
	uint32_t upper_panel_base;
	uint32_t lower_panel_base;
	uint32_t mask_set_clear;
	uint32_t control;
} pl110_MMIO;

typedef struct {
	pl110_MMIO * mmio;
	uint32_t * frame_buffer;
	uint32_t width;
	uint32_t height;
} pl110_Info;
/* ============================ [ DATAS     ] ====================================================== */
static pl110_Info plInfo;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static void pl110_setPixel(uint32_t x, uint32_t y, uint32_t color) {
	plInfo.frame_buffer[x + plInfo.width * y] = color;
}

static void pl110_setResolution(pl110_DisplayResolution resolution) {
	switch (resolution) {
		default:
		case PL110_640x480_25MHZ:
		plInfo.width = 640;
		plInfo.height = 480;
		plInfo.mmio->timing_0 = 0x3f1f3f9c;
		plInfo.mmio->timing_1 = 0x090b61df;
		plInfo.mmio->timing_2 = 0x067f1800;
		plInfo.mmio->mask_set_clear = 0x82b;
		break;
	}
}

static void pl110_init() {
	plInfo.mmio = (pl110_MMIO *)(ARM_PL110_BASE);
	plInfo.mmio->upper_panel_base = 0x200000;
	plInfo.frame_buffer = (uint32_t *)(plInfo.mmio->upper_panel_base);

	pl110_setResolution(PL110_640x480_25MHZ);

	uint32_t x, y = 0;
	for (x = 0; y < plInfo.height; y++)
		for (x = 0; x < plInfo.width; x++)
			pl110_setPixel(x, y, 0xff0000);
}

/* ============================ [ FUNCTIONS ] ====================================================== */
void Lcd_Init(void)
{
	pl110_init();
}

void LCD_DrawPixel( uint32 x, uint32 y, uint32 color )
{
	pl110_setPixel(x, y, color);
}
#endif /* USE_LCD */

