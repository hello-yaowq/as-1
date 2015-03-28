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
 * Sg: static GUI
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Sg.h"
#include "SgDraw.h"
#include "SgRes.h"

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static boolean sgUpdateInProcessing  = FALSE;
static uint8   sgLayer = 0;
static uint32  sgWI    = 0;
static void SgDrawBMP(SgWidget* w)
{
	const SgBMP* bmp;
	uint32 x,y,W,H,color;
	uint32 X = w->x;
	uint32 Y = w->y;

	if(w->ri < w->src->rs)
	{
		bmp = (SgBMP*)w->src->r[w->ri];
		W = bmp->w;
		H = bmp->h;

		if( (bmp->w <= W) && (bmp->h <= H) )
		{
			X += (w->w-W)/2;
			Y += (w->h-H)/2;
			for(x=0;x<W;x++)
			{
				for(y=0;y<H;y++)
				{
					color = bmp->p[y*W + x];
					Sg_DrawPixel(X+x,Y+y,color);
				}
			}
		}
		else
		{
			/* out of range */
			assert(0);
		}

	}
	else
	{
		assert(0);
	}

}
static void SgDrawWidget(SgWidget* w)
{
	switch(w->src->t)
	{
		case SGT_DMP:
			break;
		case SGT_BMP:
			SgDrawBMP(w);
			break;
		case SGT_TXT:
			break;
		default:
			assert(0);
			break;
	}
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void Sg_Init(void)
{
	uint32 x,y;
	sgLayer = 0;
	sgWI    = 0;
	sgUpdateInProcessing = FALSE;

	for(x=0;x<SG_LCD_WIGTH;x++)
	{
		for(y=0;y<SG_LCD_HEIGHT;y++)
		{
			Sg_DrawPixel(x,y,0xFDFDFD);
		}
	}


}


void Sg_ManagerTask(void)
{
	SgWidget* w;
	boolean  drawFlag = FALSE;

	if(sgUpdateInProcessing)
	{
		/* do nothing as now, only 1 buffer is used */
	}
	else
	{
		while (sgLayer < SGL_MAX)
		{	/* render 1 widget */
			while(sgWI < SGW_MAX)
			{
				w = &SGWidget[sgWI];
				sgWI ++;
				if(w->l == sgLayer)
				{
					SgDrawWidget(w);
					drawFlag = TRUE;
				}
				else
				{
					/* continue */
				}
			}
			if(SGW_MAX <= sgWI)
			{
				sgLayer ++;
				sgWI = 0;
			}

			if(drawFlag)
			{
				break;
			}
		}

		if(SGL_MAX <= sgLayer)
		{
			sgLayer = 0;
			sgUpdateInProcessing = TRUE;
		}
	}
}

boolean Sg_IsDataReady ( void )
{

	boolean isReady = sgUpdateInProcessing;

	sgUpdateInProcessing = FALSE;

	return isReady;
}
