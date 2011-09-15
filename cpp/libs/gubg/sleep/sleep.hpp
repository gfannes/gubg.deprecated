#ifndef sleep_hpp
#define sleep_hpp

namespace gubg
{
#ifdef DONT_KNOW_HOW_TO_CALL_sleep
    void sleep(int nrSec);
#endif
    void nanosleep(int nrSec, int nrNanoSec);
}

#endif
