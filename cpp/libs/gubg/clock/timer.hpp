#ifndef HEADER_gubg_clock_timer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_clock_timer_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"
#include <iostream>

namespace gubg
{
    enum class ResetType {Auto, NoAuto};
    struct Timer
    {
        Timer(ResetType resetType):
            resetType_(resetType)
        {
            reset();
        }

        //Elapsed difference in seconds
        double difference()
        {
            auto diff = durationSinceEpoch_sec() - startTime_;
            if (ResetType::Auto == resetType_)
                reset();
            return diff;
        }

        void reset()
        {
            startTime_ = durationSinceEpoch_sec();
        }

        private:
        double startTime_;
        ResetType resetType_;
    };
}

#endif
