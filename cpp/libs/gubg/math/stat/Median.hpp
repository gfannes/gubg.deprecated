#ifndef HEADER_gubg_math_stat_Median_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Median_hpp_ALREADY_INCLUDED

#include <algorithm>
#include <cassert>

#define GUBG_MODULE "Median"
#include "gubg/log/begin.hpp"
namespace gubg { namespace math { 

    //WARNING: the begin-end range might be permutated
    template <typename It>
        typename It::value_type median(It begin, It end)
        {
            S();
            const auto s = (end - begin);
            L(STREAM(s));
            assert(s != 0);
            //TODO: sorting is sub-optimal, use quickselect or binmedian/binapprox for better performance
            std::sort(begin, end);
            begin += (s-1)/2;
            if (s%2 == 1)
            {
                L("Odd");
                return *begin;
            }
            L("Even");
            const auto a = *begin;
            const auto b = *++begin;
            L(STREAM(a, b));
            return 0.5*(a+b);
        }

} } 
#include "gubg/log/end.hpp"

#endif
