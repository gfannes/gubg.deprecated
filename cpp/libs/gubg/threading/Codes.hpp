#ifndef HEADER_gubg_threading_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_threading_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg
{
    namespace threading
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            ProcessorAlreadyRunning,
            ProcessorAlreadyStopped,
        };
    }
}

#endif
