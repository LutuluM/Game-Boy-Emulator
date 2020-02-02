#include "TimeSync.hpp"
#include "CPU.h"
#include <time.h>

void timeSync()
{
	static ulong frames = 0;
	static clock_t time = 0;
	if (getCPUTicks() - frames > FRAMESPERSEC / 100)
	{
		while (clock() - time < 10)
			;
		time = clock();
		frames = getCPUTicks();
	}
}