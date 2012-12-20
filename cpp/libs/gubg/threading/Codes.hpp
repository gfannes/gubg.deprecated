#ifndef gubg_threading_Codes_hpp
#define gubg_threading_Codes_hpp

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
