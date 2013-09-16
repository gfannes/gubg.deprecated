#ifndef HEADER_gubg_math_Norm_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_Norm_hpp_ALREADY_INCLUDED

#include <cmath>

namespace gubg
{
    namespace math
    {
        template <typename Ctr>
            double l2Norm2(const Ctr &ctr)
            {
                double res = 0.0;
                for (auto e: ctr)
                    res += e*e;
                return res;
            }
        template <typename Ctr>
            double l2Norm(const Ctr &ctr)
            {
                return std::sqrt(l2Norm2(ctr));
            }
    }
}

#endif
