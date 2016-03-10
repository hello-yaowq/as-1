
#include "Sg.h"
#include "Stmo.h"
void* RefreshClusterTachoPointer(SgWidget* w)
{
	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_TACHO,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;

	return 0;
}

void* RefreshClusterSpeedPointer(SgWidget* w)
{
	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_SPEED,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;

	return 0;
}

void* RefreshClusterTempPointer(SgWidget* w)
{

	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_TEMP,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;
	return 0;
}


void* RefreshClusterFuelPointer(SgWidget* w)
{

	Stmo_DegreeType degree;

	Stmo_GetPosDegree(STMO_ID_FUEL,&degree);

	w->d = (degree/STMO_ONE_DEGREE)%360;
	return 0;
}
