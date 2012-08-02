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
            ResizeFailed, StartMarkerMissing, PlainTooShort, PlainTooLong, EndMarkerMissing,
            IllegalEncoding, UnknownControlReceived, NoPlainSet, StartMarkerExpected, TooManyDx,
            UnexpectedEndReceived, EndMarkerExpected, PlainNotEmpty, FirstByteCannotBeFixPos,
        };
    }
}

#endif
