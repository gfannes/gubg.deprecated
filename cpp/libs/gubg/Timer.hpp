#ifndef aoeaoeu
#define aoeaoeu

#include "gubg/crtp.hpp"
#include <chrono>

namespace gubg
{
    template <typename Receiver, typename Clock = std::chrono::high_resolution_clock>
        class Timer_crtp
        {
            public:
                typedef Clock clock;

                Timer_crtp():
                    isExpired_(false),
                    expireTime_(clock::now()){}

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
                    receiver_().timer_expired();
                }

            private:
                GUBG_RECEIVER();
                typename clock::time_point expireTime_;
                typename clock::duration timeout_;
                bool isExpired_;
        };
}

#endif
