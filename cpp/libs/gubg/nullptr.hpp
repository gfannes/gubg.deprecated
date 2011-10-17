#ifndef gubg_nullptr_hpp
#define gubg_nullptr_hpp

#include "gubg/Platform.hpp"

#if GCC_VERSION < 40600
#define nullptr (0)

namespace std
{
    typedef void * nullptr_t;
}
#endif

#endif
