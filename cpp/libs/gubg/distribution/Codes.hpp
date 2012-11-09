#ifndef HEADER_gubg_distribution_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Codes_hpp_ALREADY_INCLUDED

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
