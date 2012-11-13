#ifndef HEADER_gubg_parse_cpp_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Types_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include "gubg/SmartRange.hpp"
#include <string>

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            typedef SmartRange<std::string> Range;

            enum class ReturnCode
            {
                MSS_DEFAULT_CODES,
                LexingFailed, RangeIsEmpty, IllegalInclude, IllegalIncludeSize, UnexpectedToken, UnexpectedSymbol,
            };
        }
    }
}

#endif
