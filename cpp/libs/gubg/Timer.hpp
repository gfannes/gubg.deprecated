#ifndef HEADER_gubg_Timer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Timer_hpp_ALREADY_INCLUDED

#include <chrono>

namespace gubg
{
    template <typename Outer, typename Clock = std::chrono::high_resolution_clock>
        class Timer_ftop
        {
            public:
                typedef Clock clock;

                Timer_ftop(Outer &outer): outer(outer) {}
                template <typename Timeout>
                    Timer_ftop(Outer &outer, Timeout timeout): outer(outer), timeout_(timeout) {}

                template <typename Timeout>
                    void setTimeout(Timeout timeout) { timeout_ = timeout; }

                void reset()
                {
                    isExpired_ = false;
                    expireTime_ = clock::now() + timeout_;
                }

                void process()
                {
                    if (isExpired_)
                        return;
                    if (clock::now() < expireTime_)
                        return;
                    isExpired_ = true;
                    outer.timer_expired();
                }

            protected:
                Outer &outer;

            private:
                typename clock::time_point expireTime_ = clock::now();
                typename clock::duration timeout_;
                bool isExpired_ = false;
        };
}

#endif
