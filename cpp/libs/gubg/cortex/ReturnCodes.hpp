#ifndef HEADER_gubg_cortex_ReturnCodes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_ReturnCodes_hpp_ALREADY_INCLUDED

#define MSS_DEBUG
#include "gubg/mss.hpp"

namespace gubg
{
    namespace cortex
    {
        enum class ReturnCode
        {
            MSS_DEFAULT_CODES,
            InputSizeError,
        };
    }
}

#endif
