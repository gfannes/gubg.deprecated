#ifndef HEADER_gubg_algorithm_hpp_ALREADY_INCLUDED
#define HEADER_gubg_algorithm_hpp_ALREADY_INCLUDED

#ifdef ARDUINO
namespace std { 
    template <typename T>
        void swap(T &lhs, T &rhs)
        {
            T tmp = lhs;
            lhs = rhs;
            rhs = tmp;
        }
} 
#else
#include <algorithm>
#endif

namespace gubg { 

    template <typename T>
        const T &maximum(const T &lhs, const T &rhs)
        {
            return (lhs < rhs) ? rhs : lhs;
        }
} 

#endif
