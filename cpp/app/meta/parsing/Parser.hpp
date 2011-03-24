#ifndef meta_parsing_Parser_hpp
#define meta_parsing_Parser_hpp

#include "parsing/Structure.hpp"
#include <string>

namespace meta
{
    class Parser
    {
        public:

            bool parse(Structure &result, const std::string &code);
    };
}

#endif
