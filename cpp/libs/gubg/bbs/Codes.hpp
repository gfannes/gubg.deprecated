#ifndef HEADER_gubg_bbs_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bbs_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace bbs { 

    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        NoNewEntries, TagValueAlreadyExists, AgentAlreadyPresent,
    };

} } 

#endif
