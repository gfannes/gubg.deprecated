#include "gubg/chrono/Uptime.hpp"
#include <chrono>

namespace  { 
    struct Uptime_
    {
        typedef std::chrono::system_clock Clock;
        const Clock::time_point start;

        Uptime_(): start(Clock::now()) { }

        double to_dbl() const
        {
            const double elapse = (Clock::now() - start).count();
            return elapse*Clock::period::num/Clock::period::den;
        }
    };
    const Uptime_ uptime_;
} 

namespace gubg { namespace chrono { 

    double uptime()
    {
        return uptime_.to_dbl();
    }

} } 
