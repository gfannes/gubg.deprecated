#ifndef HEADER_garf_Types_stream_hpp_ALREADY_INCLUDED
#define HEADER_garf_Types_stream_hpp_ALREADY_INCLUDED

#include "garf/Types.hpp"
#include <ostream>

namespace garf
{
    namespace pod
    {
        inline std::ostream &operator<<(std::ostream &os, const Time &t)
        {
            os << "Time: " << t.millis << " ms";
            return os;
        }
        inline std::ostream &operator<<(std::ostream &os, const TopInfo &ti)
        {
            os << "TopInfo: nrLoops:" << ti.nrLoops << ", maxElapse:" << ti.maxElapse;
            return os;
        }
    }
}

#endif
