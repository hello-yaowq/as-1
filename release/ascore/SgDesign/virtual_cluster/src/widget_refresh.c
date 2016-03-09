
#include "Sg.h"
void* RefreshClusterTachoPointer(SgWidget* w)
{
	static uint32_t degree=0;

	degree += 200;

	w->d = (degree/100)%360;

	return 0;
}

void* RefreshClusterSpeedPointer(SgWidget* w)
{
	static uint32_t degree=0;

	degree += 150;

	w->d = (degree/100)%360;

	return 0;
}
