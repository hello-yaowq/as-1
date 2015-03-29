#include "SgRes.h"
#include <time.h>
void RefreshPointerHour(SgWidget* w)
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
}

void RefreshPointerMinute(SgWidget* w)
{
	time_t t = time(0);
	struct tm* lt = localtime(&t);

	if(lt->tm_min < 15)
	{
		w->d = 360 - (15 - lt->tm_min)*6;
	}
	else
	{
		w->d = (lt->tm_min-15)*6;
	}

	w->d += lt->tm_sec*6/60;
}

void RefresPointerSecond(SgWidget* w)
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
}
