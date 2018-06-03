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
#include "Lcd.h"

#include "asdebug.h"

#ifdef USE_LVGL
#include "lvgl/lvgl.h"
#endif
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
	for (y = 0; y < plInfo.height; y++)
		for (x = 0; x < plInfo.width; x++)
			pl110_setPixel(x, y, 0xff0000);
}

#ifdef USE_LVGL
static void lcd_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
   /*Return if the area is out the screen*/
	if(x2 < 0) return;
	if(y2 < 0) return;
	if(x1 > plInfo.width - 1) return;
	if(y1 > plInfo.height - 1) return;

	/*Truncate the area to the screen*/
	int32_t act_x1 = x1 < 0 ? 0 : x1;
	int32_t act_y1 = y1 < 0 ? 0 : y1;
	int32_t act_x2 = x2 > plInfo.width - 1 ? plInfo.width - 1 : x2;
	int32_t act_y2 = y2 > plInfo.height - 1 ? plInfo.height - 1 : y2;

	int32_t x;
	int32_t y;
	uint32_t color24 = lv_color_to24(color);

	for(x = act_x1; x <= act_x2; x++) {
		for(y = act_y1; y <= act_y2; y++) {
			plInfo.frame_buffer[y * plInfo.width + x] = color24;
		}
	}
}

static void lcd_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
   /*Return if the area is out the screen*/
	if(x2 < 0) return;
	if(y2 < 0) return;
	if(x1 > plInfo.width - 1) return;
	if(y1 > plInfo.height - 1) return;

	/*Truncate the area to the screen*/
	int32_t act_x1 = x1 < 0 ? 0 : x1;
	int32_t act_y1 = y1 < 0 ? 0 : y1;
	int32_t act_x2 = x2 > plInfo.width - 1 ? plInfo.width - 1 : x2;
	int32_t act_y2 = y2 > plInfo.height - 1 ? plInfo.height - 1 : y2;

	int32_t x;
	int32_t y;

	for(y = act_y1; y <= act_y2; y++) {
		for(x = act_x1; x <= act_x2; x++) {
			plInfo.frame_buffer[y * plInfo.width + x] = lv_color_to24(*color_p);
			color_p++;
		}

		color_p += x2 - act_x2;
	}

}

static void lcd_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > plInfo.width - 1 || y1 > plInfo.height - 1) {
        lv_flush_ready();
        return;
    }

    int32_t y;
#if LV_COLOR_DEPTH != 24
    int32_t x;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
        	plInfo.frame_buffer[y * plInfo.width + x] = lv_color_to24(*color_p);
            color_p++;
        }

    }
#else
    uint32_t w = x2 - x1 + 1;
    for(y = y1; y <= y2; y++) {
        memcpy(&plInfo.frame_buffer[y * plInfo.width + x1], color_p, w * sizeof(lv_color_t));

        color_p += w;
    }
#endif

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_flush_ready();
}
#endif
/* ============================ [ FUNCTIONS ] ====================================================== */
void Lcd_Init(void)
{
	pl110_init();
}

void LCD_DrawPixel( uint32 x, uint32 y, uint32 color )
{
	pl110_setPixel(x, y, color);
}

#ifdef USE_LVGL
void lv_hw_dsp_init(void)
{
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.disp_flush = lcd_flush;
	disp_drv.disp_fill = lcd_fill;
	disp_drv.disp_map = lcd_map;
	lv_disp_drv_register(&disp_drv);
}
#endif
#endif /* USE_LCD */

