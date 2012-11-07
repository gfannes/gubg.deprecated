#ifndef da_finalize_hpp
#define da_finalize_hpp

#include "gubg/l.hpp"
#include <cstdlib>

#define DA_FINALIZE(code, msg) \
do { \
    L(msg); \
    std::exit(code); \
} while (false)
#define DA_FINALIZE_OK(msg) DA_FINALIZE(0, msg)
#define DA_FINALIZE_ERROR(msg) DA_FINALIZE(-1, "ERROR::" << msg)

#endif
