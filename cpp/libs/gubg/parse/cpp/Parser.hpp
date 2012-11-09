#ifndef HEADER_gubg_parse_cpp_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/Types.hpp"
#include "gubg/file/File.hpp"

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            class Parser
            {
                public:
                    ReturnCode parse(const file::File &);

                private:
                    ReturnCode parseNonRecursive_();

                    Range range_;
            };
        }
    }
}

#endif
