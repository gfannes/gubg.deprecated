#ifndef HEADER_gubg_parse_basic_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_basic_hpp_ALREADY_INCLUDED

#include <string>
#include <deque>

namespace gubg
{
    namespace parse
    {
        bool startsWith(const std::string &haystack, const std::string &needle);
        std::deque<std::string> tokenize(const std::string &str, const std::string &token);
    }
}

#endif
