#ifndef gubg_msgpack_MsgPack_hpp
#define gubg_msgpack_MsgPack_hpp

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
