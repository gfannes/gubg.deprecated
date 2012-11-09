#ifndef HEADER_gubg_asn1_Error_hpp_ALREADY_INCLUDED
#define HEADER_gubg_asn1_Error_hpp_ALREADY_INCLUDED

#include "Exception.hpp"

namespace gubg
{
    namespace asn1
    {
        struct Error: Exception
        {
            Error(const std::string &msg):
                Exception(msg){}
        };
    }
}

#endif
