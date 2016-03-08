
#include "Sg.h"
#include <time.h>
void* RefreshClusterTachoPointer(SgWidget* w)
{
	time_t t = time(0);
	struct tm* lt = localtime(&t);

	if(lt->tm_sec < 15)
	{
		w->d = 360 - (15 -lt->tm_sec)*6;
	}
	else
	{
		w->d = (lt->tm_sec-15)*6;
	}

	return 0;
}

void* RefreshClusterSpeedPointer(SgWidget* w)
{
	time_t t = time(0);
	struct tm* lt = localtime(&t);

	if(lt->tm_sec < 15)
	{
		w->d = 360 - (15 -lt->tm_sec)*6;
	}
	else
	{
		w->d = (lt->tm_sec-15)*6;
	}

	return 0;
}
