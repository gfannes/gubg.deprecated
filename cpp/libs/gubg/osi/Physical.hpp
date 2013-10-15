#ifndef HEADER_gubg_osi_Physical_hpp_ALREADY_INCLUDED
#define HEADER_gubg_osi_Physical_hpp_ALREADY_INCLUDED

#include "gubg/osi/Codes.hpp"
#include <iostream>

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
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
#include "gubg/log/end.hpp"

#endif
