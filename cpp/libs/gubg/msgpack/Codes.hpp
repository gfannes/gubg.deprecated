#ifndef gubg_msgpack_Codes_hpp
#define gubg_msgpack_Codes_hpp

#include "gubg/mss.hpp"

namespace gubg
{
    namespace msgpack
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            OutOfRange, TooLarge, ReadError, RangeIsEmpty, UnknownType, 
            UnknownPrimitive, UnknownGroup, UnknownWidth, InvalidNr, UnsupportedWidth,
            ParsingFinished,
        };
    }
}

#endif
