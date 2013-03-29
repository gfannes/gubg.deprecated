#ifndef aoeaoeu
#define aoeaoeu

#include "gubg/chrono/Stopwatch.hpp"

namespace gubg
{
    namespace log
    {
        class ScopeTimer
        {
            public:
                ScopeTimer(const std::string &msg)
                {
                    LLL(this << " <Starting> \"" << msg << "\" ...");
                }
                ~ScopeTimer()
                {
                    LLL(this << " <Stopped> " << std::chrono::duration_cast<std::chrono::milliseconds>(sw_.mark().total_elapse()).count()*0.001 << " sec");
                }
            private:
                gubg::chrono::Stopwatch<> sw_;
        };
    }
}

#endif
