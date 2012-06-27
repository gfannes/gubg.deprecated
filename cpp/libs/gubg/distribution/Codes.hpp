#ifndef gubg_distribution_Codes_hpp
#define gubg_distribution_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace distribution
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            SizeMismatch, DivisionByZero, StdDevTooSmall,
        };
    }
}

#endif
