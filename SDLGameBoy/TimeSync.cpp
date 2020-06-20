#include "TimeSync.hpp"

void timeSync(){
    static ulong cpuCycles = 0;
    static clock_t time = 0;
    if (getCPUTicks() - cpuCycles > CYCLESPERSEC / 100)
    {
        while (clock() - time < (CLOCKS_PER_SEC / 100))
            ;
        time = clock();
        cpuCycles = getCPUTicks();
    }
}
