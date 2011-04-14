#ifndef meta_parsing_Parser_hpp
#define meta_parsing_Parser_hpp

#include "parsing/Structure.hpp"
#include "gubg/Exception.hpp"
#include <string>

namespace meta
{
    class Parser
    {
        public:
            struct EmptyCode: gubg::Exception
        {
            EmptyCode(): gubg::Exception("Source code is empty"){}
        };

            Structure::Ptr parse(Code &code);
    };
}

#endif
