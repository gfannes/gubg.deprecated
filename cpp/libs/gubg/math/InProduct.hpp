#ifndef HEADER_gubg_math_InProduct_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_InProduct_hpp_ALREADY_INCLUDED

#include "gubg/tuple/ForEach.hpp"
#include <tuple>
#include <array>

namespace gubg { namespace math { 

    template <typename Res>
        Res inproduct(const float &a, const float &b) { return a*b; }
    template <typename Res>
        Res inproduct(const double &a, const double &b) { return a*b; }
    template <typename Res>
        Res inproduct(const int &a, const int &b) { return a*b; }

    template <typename Res, typename T, size_t N>
        Res inproduct(const std::array<T, N> &a, const std::array<T, N> &b)
        {
            Res res = 0;
            for (size_t ix = 0; ix < N; ++ix)
                res += inproduct<Res>(a[ix], b[ix]);
            return res;
        }

    namespace details { 
        template <typename Res>
            struct InProd
            {
                Res res = 0.0;
                template <typename T>
                    void operator()(const T &a, const T &b)
                    {
                        res += inproduct<Res>(a, b);
                    }
            };
    } 
    template <typename Res, typename... T>
        Res inproduct(const std::tuple<T...> &a, const std::tuple<T...> &b)
        {
            details::InProd<Res> inprod;
            tuple::for_each(a, b, inprod);
        }

} } 

#endif
