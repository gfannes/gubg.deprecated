#ifndef HEADER_garf_Types_stream_hpp_ALREADY_INCLUDED
#define HEADER_garf_Types_stream_hpp_ALREADY_INCLUDED

#include "garf/Types.hpp"
#include <ostream>

namespace std
{
    inline ostream &operator<<(ostream &os, const garf::Time &t)
    {
        os << "Time: " << t.millis << " ms";
        return os;
    }
    inline ostream &operator<<(ostream &os, const garf::TopInfo &ti)
    {
        os << "TopInfo: nrLoops:" << ti.nrLoops << ", maxElapse:" << ti.maxElapse;
        return os;
    }
}

#endif