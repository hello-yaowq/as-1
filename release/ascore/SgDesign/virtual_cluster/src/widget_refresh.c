
#include "Sg.h"
#include <time.h>
void* RefreshPointerHour(SgWidget* w)
{
	time_t t = time(0);
	struct tm* lt = localtime(&t);

	if(lt->tm_hour < 3)
	{
		w->d = 360 - (3-lt->tm_hour)*30;
	}
	else
	{
		w->d = (lt->tm_hour-3)*30;
	}

	w->d += lt->tm_min*30/60;

	return 0;
}
