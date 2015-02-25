#ifndef HEADER_gubg_wav_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace wav { 

    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        UnsupportFormat, UnsupportBitsPerSample,
    };

} } 

#endif
