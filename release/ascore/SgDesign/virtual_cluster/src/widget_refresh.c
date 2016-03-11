
#include "Sg.h"
#include "Stmo.h"

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
		w->l = 1;
	}
	else if(tflash < 40)
	{
		w->l = SGL_INVALID;
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
