#ifndef HEADER_gubg_msgpack_MsgPack_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_MsgPack_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Codes.hpp"
#include "gubg/msgpack/Write.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename Backend>
            class Buffer
            {
                public:
                    Backend backend_;
            };
    }
}

#endif
