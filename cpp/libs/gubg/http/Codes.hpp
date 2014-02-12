#ifndef HEADER_gubg_http_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace http { 

    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        ResponseWasSent, NotEnoughData,
    };

} } 

#endif
