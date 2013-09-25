#ifndef HEADER_gubg_math_Norm_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_Norm_hpp_ALREADY_INCLUDED

#include <cmath>
#include <algorithm>

namespace gubg
{
    namespace math
    {
        //Checks if wanted is equal to actual within a given precision
        template <typename T>
            bool isAlmost(T wanted, T actual, T precision)
            {
                const auto diff = std::abs(actual-wanted);
                return diff <= precision;
            }

        namespace l2
        {
            template <typename Ctr>
                double norm_sqr(const Ctr &ctr)
                {
                    double res = 0.0;
                    for (auto e: ctr)
                        res += e*e;
                    return res;
                }
            template <typename Ctr>
                double norm(const Ctr &ctr)
                {
                    return std::sqrt(norm_sqr(ctr));
                }

            template <typename Lhs, typename Rhs>
                double distance_sqr(const Lhs &lhs, const Rhs &rhs)
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
                double distance(const Lhs &lhs, const Rhs &rhs)
                {
                    return std::sqrt(distance_sqr(lhs, rhs));
                }
        }

        namespace l1
        {
            template <typename Ctr>
                double norm(const Ctr &ctr)
                {
                    double res = 0.0;
                    for (auto e: ctr)
                        res += std::abs(e);
                    return res;
                }
            template <typename Ctr>
                void normalize(Ctr &ctr)
                {
                    const auto factor = 1.0/norm(ctr);
                    std::transform(RANGE(ctr), ctr.begin(), [&](double d){return factor*d;});
                }
            template <typename Ctr>
                void normalize(Ctr &ctr, typename Ctr::value_type wantedNorm)
                {
                    const auto factor = wantedNorm/norm(ctr);
                    std::transform(RANGE(ctr), ctr.begin(), [&](double d){return factor*d;});
                }
        }
    }
}

#endif
