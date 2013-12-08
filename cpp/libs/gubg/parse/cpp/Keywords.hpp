#ifndef HEADER_gubg_parse_cpp_Keywords_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Keywords_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/Types.hpp"

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            bool isKeyword(const std::string &str);
            inline bool isKeyword(const Range &range) {return isKeyword(range.content());}
        }
    }
}

#endif
