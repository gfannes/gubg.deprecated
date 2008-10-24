#ifndef clock_hpp
#define clock_hpp

#include <sys/time.h>

#include "timestamp.hpp"

// A clock that can be used for absolute time retrieval and elapse time computation since its creation
class Clock
{
public:
    Clock():
        mOffset()
        {
            mStart.now();
        }
    // A singleton local clock
    static Clock &local();
    bool now(TimeStamp &ts) const;
    TimeStamp elapse()
        {
            TimeStamp end;
            end.now();
            end.diff(mStart);
            return end;
        }

    friend ostream &operator<<(ostream &os, const Clock &clock);
private:
    static Clock *sLocalClock;
    TimeStamp mOffset;
    TimeStamp mStart;
};

#endif
