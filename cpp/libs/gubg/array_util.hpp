#ifndef HEADER_gubg_Array_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Array_hpp_ALREADY_INCLUDED

#include <array>

namespace gubg
{
    namespace priv
    {
        template <typename T>
            void fillArray(T *ary) { }
        template <typename T, typename... Rest>
            void fillArray(T *ary, T v, Rest... rest)
            {
                *ary = v;
                fillArray(ary+1, rest...);
            }
    }

    template <typename T, typename... Rest>
        std::array<T, sizeof...(Rest)+1> make_array(T v, Rest... rest)
        {
            std::array<T, sizeof...(rest)+1> ret;
            ret[0] = v;
            priv::fillArray(ret.data()+1, rest...);
            return ret;
        }
}

#endif
