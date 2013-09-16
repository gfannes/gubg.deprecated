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

        template <typename Lhs, typename Rhs>
            double l2Dist2(const Lhs &lhs, const Rhs &rhs)
            {
                double res = 0.0;
                assert(lhs.size() == rhs.size());
                auto it = rhs.begin();
                double tmp;
                for (auto l: lhs)
                {
                    tmp = (l-*it++);
                    res += tmp*tmp;
                }
                return res;
            }
        template <typename Lhs, typename Rhs>
            double l2Dist(const Lhs &lhs, const Rhs &rhs)
            {
                return std::sqrt(l2Dist2(lhs, rhs));
            }
    }
}

#endif
