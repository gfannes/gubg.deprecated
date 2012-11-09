#ifndef sadfs
#define sadfs
#ifdef L_ENABLE_DEBUG
#include <iostream>
#define DEBUG_PRINT(msg) std::cout << msg << std::endl
#else
#define DEBUG_PRINT(msg)
#endif
#endif
