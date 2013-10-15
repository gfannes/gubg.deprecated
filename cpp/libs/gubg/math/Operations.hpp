#ifndef HEADER_gubg_math_Operations_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_Operations_hpp_ALREADY_INCLUDED

#include "gubg/math/Codes.hpp"

#define GUBG_MODULE "Operations"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace math
    {
        template <typename T, typename TT>
            ReturnCode divide(T &quotient, T &remainder, const TT nominator, const TT denominator)
            {
                static_assert(sizeof(T) == sizeof(TT), "Equal sizes expected between types");
                MSS_BEGIN(ReturnCode);
                MSS(nominator >= 0, OnlyUnsignedDivisionForNow);
                MSS(denominator >= 0, OnlyUnsignedDivisionForNow);
                MSS(denominator > 0, DivisionByZero);
                quotient = remainder = 0;
                for (int i = sizeof(T)*8-1; i >= 0; --i)
                {
                    remainder <<= 1;
                    const T mask = (1 << i);
                    if (mask & nominator)
                        ++remainder;
                    if (remainder >= denominator)
                    {
                        remainder -= denominator;
                        quotient |= mask;
                    }
                }
                MSS_END();
            }
    }
}
#include "gubg/log/end.hpp"

#endif
