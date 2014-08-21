#ifndef HEADER_didi_Finalize_hpp_ALREADY_INCLUDED
#define HEADER_didi_Finalize_hpp_ALREADY_INCLUDED

#include <cstdlib>
#include <iostream>

#define DIDI_FINALIZE(code, msg) \
do { \
    std::cout << msg << std::endl; \
    std::exit(code); \
} while (false)
#define DIDI_FINALIZE_OK(msg)    DIDI_FINALIZE(0, msg)
#define DIDI_FINALIZE_ERROR(msg) DIDI_FINALIZE(-1, "ERROR::" << msg)

#endif
