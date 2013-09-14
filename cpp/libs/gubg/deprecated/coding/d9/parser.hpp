#ifndef HEADER_gubg_deprecated_coding_d9_parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_deprecated_coding_d9_parser_hpp_ALREADY_INCLUDED

//A 2-phase parser:
// * First, add bytes
// * When a complete message has been received, create() can parse it into a Package

#include "gubg/coding/d9/package.hpp"
#include "gubg/chunky.hpp"
#include <list>

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            class Parser
            {
                public:
                    Parser();

                    ReturnCode add(ubyte);
                    ReturnCode create(Package &);

                    void clear();

                private:
                    enum State {Idle, Sd9, Meta, Attributes, Body, Ed9, Checksum};
                    State state_;
                    list<Chunky> messages_;
            };
        }
    }
}

#endif
