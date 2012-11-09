#ifndef HEADER_gubg_d9_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_d9_Codes_hpp_ALREADY_INCLUDED

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
            UnexpectedD9Received, EndMarkerExpected, PlainNotEmpty, FirstByteCannotBeFixPos, TooManyFlips,
            ContentComplete, UnexpectedReply, PushBackFailed, IllegalContent,
        };
    }
}

#endif
