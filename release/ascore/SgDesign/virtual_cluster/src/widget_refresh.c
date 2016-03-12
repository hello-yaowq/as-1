
#include "Sg.h"
#include "Stmo.h"

#define TT_ON  1
#define TT_OFF SGL_INVALID

void CacheClusterTachoPointer(SgWidget* w){}
void* RefreshClusterTachoPointer(SgWidget* w)
{
	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_TACHO,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;

	return 0;
}


void CacheClusterSpeedPointer(SgWidget* w){}
void* RefreshClusterSpeedPointer(SgWidget* w)
{
	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_SPEED,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;

	return 0;
}

void CacheClusterTempPointer(SgWidget* w){}
void* RefreshClusterTempPointer(SgWidget* w)
{

	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_TEMP,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;
	return 0;
}

void CacheClusterFuelPointer(SgWidget* w){}
void* RefreshClusterFuelPointer(SgWidget* w)
{

	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_FUEL,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;
	return 0;
}

void CacheTelltaleTPMS(SgWidget* w)
{

	static int tflash=0;

	tflash ++;

	if(tflash < 20)
	{
		w->l = TT_ON;
	}
	else if(tflash < 40)
	{
		w->l = TT_OFF;
	}
	else
	{
		w->l = 1;
		tflash = 0;
	}
}

void* RefreshTelltaleTPMS(SgWidget* w)
{
	return 0;
}

void CacheTelltaleLowOil(SgWidget* w)
{

	static int tflash=0;

	tflash ++;

	if(tflash < 30)
	{
		w->l = TT_ON;
	}
	else if(tflash < 60)
	{
		w->l = TT_OFF;
	}
	else
	{
		w->l = 1;
		tflash = 0;
	}
}

void* RefreshTelltaleLowOil(SgWidget* w)
{
	return 0;
}
