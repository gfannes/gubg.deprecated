#ifndef gubg_msgpack_Write_hpp
#define gubg_msgpack_Write_hpp

#include "gubg/msgpack/Codes.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename Buffer>
            ReturnCode write(Buffer &buffer, int v)
            {
                MSS_BEGIN(ReturnCode);
                if (v >= 0)
                {
                    if (v <= 0x7f)
                        buffer.push_back((char)v);
                    else if (v <= 0xff)
                    {
                        buffer.push_back(0xcc);
                        buffer.push_back((char)v);
                    }
                    else if (v <= 0xffff)
                    {
                        buffer.push_back(0xcd);
                        buffer.push_back((char)((v >> 8) & 0xff));
                        buffer.push_back((char)(v & 0xff));
                    }
                    else if (v <= 0xffffffff)
                    {
                        buffer.push_back(0xce);
                        buffer.push_back((char)((v >> 24) & 0xff));
                        buffer.push_back((char)((v >> 16) & 0xff));
                        buffer.push_back((char)((v >> 8) & 0xff));
                        buffer.push_back((char)(v & 0xff));
                    }
                    else
                        MSS_L(OutOfRange);
                }
                else
                {
                    MSS_L(OutOfRange);
                }
                MSS_END();
            }
    }
}

#endif
