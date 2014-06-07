#ifndef HEADER_gubg_cstdint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cstdint_hpp_ALREADY_INCLUDED

#ifdef ARDUINO

namespace std { 
    typedef unsigned char uint8_t;
} 

#else

#include <cstdint>

#endif

#endif
