#ifndef HEADER_da_Codes_hpp_ALREADY_INCLUDED
#define HEADER_da_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace da
{
    enum class ReturnCode
    {
        MSS_DEFAULT_CODES,
        Skip, Stop, CompilationFailed, LinkingFailed, HeaderAlreadyKnown,
        AvrObjCopyFailed, AvrDudeFailed, RecognisedHeader, UnknownHeader,
        NoIncludeGuardFound, RunFailed,
    };
}

#endif
