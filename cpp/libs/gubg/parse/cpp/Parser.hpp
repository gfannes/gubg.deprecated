#ifndef gubg_parse_cpp_Parser_hpp
#define gubg_parse_cpp_Parser_hpp

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
            };
        }
    }
}

#endif
