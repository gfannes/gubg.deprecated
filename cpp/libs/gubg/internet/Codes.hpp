#ifndef gubg_internet_Codes_hpp
#define gubg_internet_Codes_hpp

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
            CouldNotSend, CouldNotReceive, ConnectWasClosed,
        };
    }
}

#endif
