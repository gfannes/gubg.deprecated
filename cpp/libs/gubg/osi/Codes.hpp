#ifndef HEADER_gubg_osi_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_osi_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace osi
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            SendPending, Sending, Receiving, Proceed,
        };
    }
}

#endif
