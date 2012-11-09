#ifndef HEADER_gubg_tcp_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tcp_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace tcp
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            , UnexpectedValidSocketDescriptor,
            UnexpectedRole, AcceptFailed, ReceiveFailure, 
        };
    }
}

#endif
