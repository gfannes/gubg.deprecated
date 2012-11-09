#ifndef HEADER_gubg_sleep_sleep_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sleep_sleep_hpp_ALREADY_INCLUDED

namespace gubg
{
#ifdef DONT_KNOW_HOW_TO_CALL_sleep
    void sleep(int nrSec);
#endif
    //1000000 nanosec == 1 millisec
    void nanosleep(int nrSec, int nrNanoSec);
}

#endif
