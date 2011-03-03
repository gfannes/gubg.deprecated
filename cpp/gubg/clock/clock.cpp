#include "clock.hpp"
#include <ostream>
using namespace std;

Clock *Clock::sLocalClock = NULL;

Clock &Clock::local()
{
    if (!sLocalClock)
        sLocalClock = new Clock();
    return *sLocalClock;
}

bool Clock::now(TimeStamp &ts) const
{
    return ts.now() && ts.add(offset_);
}

ostream &operator<<(ostream &os, const Clock &clock)
{
    TimeStamp ts;
    clock.now(ts);
    os << "Clock time: " << ts;
}
