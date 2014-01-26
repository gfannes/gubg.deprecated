#ifndef HEADER_rip_Codes_hpp_ALREADY_INCLUDED
#define HEADER_rip_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace rip { 
    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        InputNotSpecified, InputFileNotFound,
    };
} 

#endif
