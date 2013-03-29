#ifndef HEADER_da_Finalize_hpp_ALREADY_INCLUDED
#define HEADER_da_Finalize_hpp_ALREADY_INCLUDED

#include <cstdlib>
#include <iostream>

#define DA_FINALIZE(code, msg) \
do { \
    std::cout << msg << std::endl; \
    std::exit(code); \
} while (false)
#define DA_FINALIZE_OK(msg) DA_FINALIZE(0, msg)
#define DA_FINALIZE_ERROR(msg) DA_FINALIZE(-1, "ERROR::" << msg)

#endif
