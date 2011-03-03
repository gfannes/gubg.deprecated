#ifndef clock_hpp
#define clock_hpp

#include "timestamp.hpp"
#include <sys/time.h>
#include <ostream>

// A clock that can be used for absolute time retrieval and elapse time computation since its creation
class Clock
{
    public:
        Clock()
        {
            start_.now();
        }
        // A singleton local clock
        static Clock &local();
        bool now(TimeStamp &ts) const;
        TimeStamp elapse()
        {
            TimeStamp end;
            end.now();
            end.diff(start_);
            return end;
        }

        friend std::ostream &operator<<(std::ostream &os, const Clock &clock);
    private:
        static Clock *sLocalClock;
        TimeStamp offset_;
        TimeStamp start_;
};

#endif
