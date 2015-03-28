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
static void SgDrawBMP(SgWidget* w)
{
	const SgBMP* bmp = (SgBMP*)w->r[w->ri];	/* TODO:no check index out of range */
	uint32 X = w->x;
	uint32 Y = w->y;
	uint32 W = bmp->w;
	uint32 H = bmp->h;
	uint32 x,y;
	uint32 color;

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
static void SgDrawWidget(SgWidget* w)
{
	switch(w->r[w->ri]->t) /* TODO: no check index out of range */
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

}


void Sg_ManagerTask(void)
{
	uint8  layer;
	uint32 i;
	SgWidget* w;
	for(layer=0;layer<SGL_MAX;layer++)
	{
		for(i=0;i<SGW_MAX;i++)
		{
			w = &SGWidget[i];
			if(w->l == layer)
			{
				SgDrawWidget(w);
			}
		}
	}
}
