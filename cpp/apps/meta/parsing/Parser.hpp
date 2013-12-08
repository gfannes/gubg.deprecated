#ifndef HEADER_meta_parsing_Parser_hpp_ALREADY_INCLUDED
#define HEADER_meta_parsing_Parser_hpp_ALREADY_INCLUDED

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

            //Parses a block from ComponentRange:
            // * Until the range is empty
            // * An extra closing '}' is found
            static Components parseBlock(ComponentRange &);
    };
}

#endif
