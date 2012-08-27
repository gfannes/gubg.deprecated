#ifndef gubg_tcp_Codes_hpp
#define gubg_tcp_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace tcp
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            CouldNotGetSocketDescriptor, PortTooLow, PortTooHigh, CouldNotBind, UnexpectedValidSocketDescriptor,
            UnexpectedRole, CouldNotListen,
        };
    }
}

#endif
