#ifndef gubg_math_Codes_hpp
#define gubg_math_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace math
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            OnlyUnsignedDivisionForNow, DivisionByZero,
        };
    }
}

#endif
