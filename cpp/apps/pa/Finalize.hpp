#ifndef HEADER_pa_Finalize_hpp_ALREADY_INCLUDED
#define HEADER_pa_Finalize_hpp_ALREADY_INCLUDED

#include <cstdlib>
#include <iostream>

#define PA_FINALIZE(code, msg) \
do { \
    std::cout << msg << std::endl; \
    std::exit(code); \
} while (false)
#define PA_FINALIZE_OK(msg) PA_FINALIZE(0, msg)
#define PA_FINALIZE_ERROR(msg) PA_FINALIZE(-1, "ERROR::" << msg)

#endif
