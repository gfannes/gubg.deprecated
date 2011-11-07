#ifndef gubg_parse_basic_hpp
#define gubg_parse_basic_hpp

#include <string>
#include <vector>

namespace gubg
{
    namespace parse
    {
        bool startsWith(const std::string &haystack, const std::string &needle);
        std::vector<std::string> tokenize(const std::string &str, const std::string &token);
    }
}

#endif
