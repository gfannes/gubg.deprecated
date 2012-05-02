#ifndef gubg_sleep_hpp
#define gubg_sleep_hpp

namespace gubg
{
#ifdef DONT_KNOW_HOW_TO_CALL_sleep
    void sleep(int nrSec);
#endif
    //1000000 nanosec == 1 millisec
    void nanosleep(int nrSec, int nrNanoSec);
}

#endif
