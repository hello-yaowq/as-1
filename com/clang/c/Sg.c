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
#include <math.h>

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */

/* ============================ [ DATAS     ] ====================================================== */
static boolean sgUpdateInProcessing  = FALSE;
static uint8   sgLayer = 0;
static uint32  sgWI    = 0;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static void SgDrawBMP0(SgWidget* w)
{
	const SgBMP* bmp;
	uint32 x,y,W,H,color;
	uint32 X;
	uint32 Y;
	bmp = (SgBMP*)w->src->r[w->ri];
	W = bmp->w;
	H = bmp->h;
	X = w->x;
	Y = w->y;

	if( (w->w >= W) && (w->h >= H) )
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
void Sg_Calc(uint32 *px,uint32 *py,uint32 cx,uint32 cy,uint16 d)
{ /* This is difficult, I think I need a lib to do this job */
	if(d == 0)
	{
		/* output by default */
	}
	else
	{
		double O = (double)M_PI*d / (double)180;
		double COS = cos(O);
		double SIN = sin(O);
		int x = px[0];
		int y = py[0];

		px[0]= (uint32)(((double)(x-(int)cx)*COS - (double)(y-(int)cy)*SIN) + (int)cx);
		py[0]= (uint32)(((double)(x-(int)cx)*SIN + (double)(y-(int)cy)*COS) + (int)cy);
	}
}
static void SgDrawBMPd(SgWidget* w)
{
	const SgBMP* bmp;
	uint32 x,y,W,H,color;
	uint32 Xc,Yc;
	uint32 cx,cy;
	bmp = (SgBMP*)w->src->r[w->ri];
	W = bmp->w;
	H = bmp->h;
	Xc = w->x;
	Yc = w->y;
	for(x=0;x<W;x++)
	{
		for(y=0;y<H;y++)
		{
			cx = Xc + x - bmp->x;
			cy = Yc + y - bmp->y;
			Sg_Calc(&cx,&cy,Xc,Yc,w->d);
			color = bmp->p[y*W + x];
			Sg_DrawPixel(cx,cy,color);
			Sg_DrawPixel(cx+1,cy,color);
		}
	}
}
static void SgDrawBMP(SgWidget* w)
{
	if(w->ri < w->src->rs)
	{
		if(0xFFFF == w->d)
		{	/* no degree */
			SgDrawBMP0(w);
		}
		else
		{	/* draw with rotation */
			SgDrawBMPd(w);
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
					if(NULL != w->src->rf)
					{
						w->src->rf(w);
					}
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
