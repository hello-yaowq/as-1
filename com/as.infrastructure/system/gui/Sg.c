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
#ifdef USE_GUI
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Sg.h"
#include "SgDraw.h"
#include "SgRes.h"
#include <math.h>

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static boolean sgUpdateInProcessing  = FALSE;
static uint8   sgLayer = 0;
static uint32  sgWI    = 0;
static uint32  sgX     = 0;
/* ============================ [ LOCALS    ] ====================================================== */
static void	SgDrawDot(uint32 x, uint32 y,const uint8* d,uint32 c)
{
	uint8 w,h;
	uint32 X,Y;
	w = d[0];
	h = d[1];
	d = d + 2;
	for(Y=0;Y<h;Y++)
	{
		for(X=0;X<w;X++)
		{
			uint8 Dot = d[Y*((w+7)/8)+X/8];
			if(Dot&(1<<(X&7)))
			{
				Sg_DrawPixel(X+x,Y+y,c);
			}
		}
	}
}
static const uint8* SgfLookup(const SgTXT* txt,uint16 code)
{
	uint16 low = 0, high = txt->s, mid;
    while( low <= high )
    {
        mid = ( low + high )/2;
        if( txt->l[mid] == code )
        {
            return txt->p[mid];
        }
        else if( txt->l[mid] < code )
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

	return NULL;
}
static boolean SgDrawBMP0(SgWidget* w)
{
	boolean rv;
	const SgBMP* bmp;
	uint32 x,y,W,H,color;
	uint32 X;
	uint32 Y;
	uint32 width;
	bmp = (SgBMP*)w->src->r[w->ri];
	W = bmp->w;
	H = bmp->h;
	X = w->x;
	Y = w->y;

	width = (w->src->weight+99)/100;
	x = (W+width-1)/width;

	if((sgX+x) < W)
	{
		width=sgX+x;
		rv = FALSE;
	}
	else
	{
		width = W;
		rv = TRUE;
	}


	if( (w->w >= W) && (w->h >= H) )
	{
		X += (w->w-W)/2;
		Y += (w->h-H)/2;
		for(x=sgX;x<width;x++)
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

	sgX = width;

	return rv;
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
static boolean SgDrawBMPd(SgWidget* w)
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

	return TRUE;
}
static boolean SgDrawBMP(SgWidget* w)
{
	boolean rv;
	if(NULL != w->src->rf)
	{
		w->src->rf(w);
	}
	if(w->ri < w->src->rs)
	{
		if(0xFFFF == w->d)
		{	/* no degree */
			rv=SgDrawBMP0(w);
		}
		else
		{	/* draw with rotation */
			rv=SgDrawBMPd(w);
		}
	}
	else
	{
		assert(0);
		rv = TRUE;
	}

	return rv;

}
static boolean SgDrawTXT(SgWidget* w)
{
	const SgTXT* txt;
	const uint16* utext;
	uint16 size;
	uint16 i;
	uint32 x,y;
	const uint8* d;

	if(w->ri < w->src->rs)
	{
		txt = (SgTXT*)w->src->r[w->ri];
		utext = w->src->rf(w);
		size = utext[0];
		utext ++;

		x = w->x+(w->w-txt->w*size)/2;
		y = w->y+(w->h-txt->h)/2;

		for(i=0;i<size;i++)
		{
			d = SgfLookup(txt,utext[i]);
			if(d != NULL)
			{
				SgDrawDot(x,y,d,w->c);
			}
			x += txt->w;
		}
	}
	else
	{
		assert(0);
	}

	return TRUE;
}
static boolean SgDrawWidget(SgWidget* w)
{
	boolean rv;
	switch(w->src->t)
	{
		case SGT_DMP:
			rv = TRUE;
			break;
		case SGT_BMP:
			rv = SgDrawBMP(w);
			break;
		case SGT_TXT:
			rv =SgDrawTXT(w);
			break;
		default:
			rv = TRUE;
			assert(0);
			break;
	}

	return rv;
}

static void SgCache(void)
{
	uint32 i;
	SgWidget* w;
	for(i=0;i<SGW_MAX;i++)
	{
		w = &SGWidget[i];
		if(NULL != w->src->cf)
		{
			w->src->cf(w);
		}
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
	uint32_t weight=0;

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

				if(w->l == sgLayer)
				{
					if(TRUE==SgDrawWidget(w))
					{
						sgWI++;
						sgX = 0;
						weight += w->src->weight%100;
					}
					else
					{
						weight += 100;
					}
				}
				else
				{
					/* continue */
					sgWI ++;
				}
			}
			if(SGW_MAX <= sgWI)
			{
				sgLayer ++;
				sgWI = 0;
			}

			if(weight >= 100)
			{	/* each main function call, just draw 100 */
				break;
			}
		}

		if(SGL_MAX <= sgLayer)
		{
			sgLayer = 0;
			sgUpdateInProcessing = TRUE;
			SgCache();
		}
	}
}

boolean Sg_IsDataReady ( void )
{

	boolean isReady = sgUpdateInProcessing;

	sgUpdateInProcessing = FALSE;

	return isReady;
}
#endif /* USE_GUI */
