#ifndef HEADER_gubg_cortex_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_Types_hpp_ALREADY_INCLUDED

#include <chrono>
#include <ostream>

namespace gubg
{
    namespace cortex
    {
        typedef double V;
        typedef double P;

        struct Value
        {
            V v;
            P p;

            Value(): v(0), p(0) {}
            Value(V vv, P pp): v(vv), p(pp) {}
        };

        namespace time
        {
            typedef std::chrono::high_resolution_clock Clock;
            typedef Clock::duration Offset;
            typedef Clock::duration Span;
            typedef Clock::duration Elapse;
            typedef Clock::time_point Point;
        }
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::cortex::Value &value)
    {
        return os << "Value(" << value.v << ", " << value.p << ")";
    }
}

#endif
