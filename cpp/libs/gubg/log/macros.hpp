//NO INCLUDE GUARDS HERE

#include <iostream>

#define LLL(msg) std::cout << msg << std::endl

#include "gubg/log/ScopeTimer.hpp"
#define SSS(msg) \
    std::ostringstream l_gubg_log_macros_SSS_msg__; \
    l_gubg_log_macros_SSS_msg__ << msg; \
    gubg::log::ScopeTimer l_gubg_log_macros_ScopeTimer__(l_gubg_log_macros_SSS_msg__.str())

