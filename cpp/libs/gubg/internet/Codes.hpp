#ifndef HEADER_gubg_internet_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_internet_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace internet
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            SocketAlreadyPresent, CouldNotGetSocketDescriptor, PortTooLow, PortTooHigh, CouldNotBind,
            InvalidState, CouldNotListen, CouldNotAccept, CouldNotGetAddrInfo, CouldNotConnect,
            CouldNotSend, CouldNotReceive, ConnectionWasClosed, ClientAlreadyRunning, NoClientRunning,
        };
    }
}

#endif
