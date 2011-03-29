#ifndef meta_parsing_Parser_hpp
#define meta_parsing_Parser_hpp

#include "parsing/Structure.hpp"
#include <string>

namespace meta
{
    class Parser
    {
        public:

            Structure parse(const std::string &code);
    };
}

#endif
