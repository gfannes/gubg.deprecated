#ifndef HEADER_gubg_reception_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_reception_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace reception
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            PortAlreadyWatched, CouldNotBind,
        };
    }
}

#endif
