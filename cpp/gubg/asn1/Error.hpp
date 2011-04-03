#ifndef gubg_asn1_Error_hpp
#define gubg_asn1_Error_hpp

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
