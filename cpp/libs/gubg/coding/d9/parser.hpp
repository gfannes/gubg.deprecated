#ifndef gubg_coding_d9_parser_hpp
#define gubg_coding_d9_parser_hpp

//A 2-phase parser:
// * First, add bytes
// * When a complete message has been received, create() can parse it into a Package

#include "gubg/coding/d9/package.hpp"

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            class Parser
            {
                public:
                    ReturnCode add();
                    ReturnCode create(Package &);
            };
        }
    }
}

#endif
