#ifndef gubg_osi_Physical_hpp
#define gubg_osi_Physical_hpp

#include "gubg/osi/Codes.hpp"
#include <iostream>

namespace gubg
{
    namespace osi
    {
        class StdioPhysical
        {
            public:
                template <typename Byte>
                ReturnCode sendByte(Byte byte)
                {
                    MSS_BEGIN(ReturnCode);
                    std::cout << byte;
                    MSS_END();
                }
        };
    }
}

#endif
