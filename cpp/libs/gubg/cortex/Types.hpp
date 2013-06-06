#ifndef HEADER_gubg_cortex_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_Types_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace cortex
    {
        typedef V double;
        typedef P double;

        struct Value
        {
            V v;
            P p;

            Value(): v(0), p(0) {}
        };

        namespace time
        {
            typedef std::chrono::milliseconds Offset;
            typedef std::chrono::milliseconds Span;
            typedef std::chrono::milliseconds Elapse;
            typedef std::chrono::high_resolution_clock Clock;
        }
    }
}

#endif
