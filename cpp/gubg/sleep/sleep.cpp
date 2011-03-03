#include "sleep.hpp"

#ifdef MINGW32

#include <windows.h>
namespace gubg
{
#ifdef DONT_KNOW_HOW_TO_CALL_sleep_HERE
    void sleep(int nrSec)
    {
        Sleep(1000*nrSec);
    }
#endif
    void nanosleep(int nrSec, int nrNanoSec)
    {
        Sleep(1000*nrSec + nrNanoSec%1000000);
    }
}

#else

#include <time.h>
namespace gubg
{
#ifdef DONT_KNOW_HOW_TO_CALL_sleep_HERE
    void sleep(int nrSec)
    {
        sleep(nrSec);
    }
#endif
    void nanosleep(int nrSec, int nrNanoSec)
    {
        timespec tv;
        tv.tv_sec = nrSec;
        tv.tv_nsec = nrNanoSec;
        nanosleep(&tv, NULL);
    }
}

#endif
