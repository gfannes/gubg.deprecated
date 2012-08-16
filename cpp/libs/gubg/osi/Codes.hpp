#ifndef gubg_osi_Codes_hpp
#define gubg_osi_Codes_hpp

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
