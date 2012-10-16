#ifndef gubg_parse_cpp_Types_hpp
#define gubg_parse_cpp_Types_hpp

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
            };
        }
    }
}

#endif
