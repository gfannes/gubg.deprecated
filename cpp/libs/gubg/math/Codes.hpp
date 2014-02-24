#ifndef HEADER_gubg_math_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace math {

    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        OnlyUnsignedDivisionForNow, DivisionByZero, SizeMismatch, StdDevTooSmall,
    };

} }

#endif
