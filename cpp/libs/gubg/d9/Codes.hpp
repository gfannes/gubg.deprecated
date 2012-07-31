#ifndef gubg_d9_Codes_hpp
#define gubg_d9_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace d9
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            ResizeFailed, StartMarkerMissing, PlainTooShort, PlainTooLong, EndMarkerMissing, IllegalEncoding,
            End, Ack, Nack,
            UnknownControlReceived, NoPlainSet, StartMarkerExpected, IllegalFlipFormat, UnexpectedEndReceived,
            EndMarkerExpected, PlainNotEmpty,
        };
    }
}

#endif
