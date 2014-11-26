#ifndef HEADER_gubg_nullptr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_nullptr_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"

#if defined(GCC_VERSION) && (GCC_VERSION < 40600)
#define nullptr (0)

namespace std
{
    typedef void * nullptr_t;
}
#endif

#endif
