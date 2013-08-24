#ifndef HEADER_monitor_Finalize_hpp_ALREADY_INCLUDED
#define HEADER_monitor_Finalize_hpp_ALREADY_INCLUDED

#include <cstdlib>
#include <iostream>

#define MONITOR_FINALIZE(code, msg) \
do { \
    std::cout << msg << std::endl; \
    std::exit(code); \
} while (false)
#define MONITOR_FINALIZE_OK(msg) MONITOR_FINALIZE(0, msg)
#define MONITOR_FINALIZE_ERROR(msg) MONITOR_FINALIZE(-1, "ERROR::" << msg)

#endif
