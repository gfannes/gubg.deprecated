#ifndef sleep_hpp
#define sleep_hpp

#ifdef MINGW32

#include <windows.h>
void sleep(int nrSec)
{
    Sleep(1000*nrSec);
}
void nanosleep(int nrSec, int nrNanoSec)
{
    Sleep(1000*nrSec + nrNanoSec%1000000);
}

#else

#include <time.h>

void nanosleep(int nrSec, int nrNanoSec)
{
    timespec tv;
    tv.tv_sec = nrSec;
    tv.tv_nsec = nrNanoSec;
    nanosleep(&tv, NULL);
}

#endif

#endif
