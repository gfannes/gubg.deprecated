#ifndef HEADER_garf_Time_hpp_ALREADY_INCLUDED
#define HEADER_garf_Time_hpp_ALREADY_INCLUDED

#include "garf/Types.hpp"

namespace garf
{
    namespace time
    {
        pod::Time now()
        {
            pod::Time res;
            res.millis = millis();
            return res;
        }
    }
}

#endif
