#ifndef HEADER_gubg_debug_debug_hpp_ALREADY_INCLUDED
#define HEADER_gubg_debug_debug_hpp_ALREADY_INCLUDED
#ifdef L_ENABLE_DEBUG
#include <iostream>
#define DEBUG_PRINT(msg) std::cout << msg << std::endl
#else
#define DEBUG_PRINT(msg)
#endif
#endif
