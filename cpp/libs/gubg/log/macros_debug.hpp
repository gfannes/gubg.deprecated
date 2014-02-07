//NO INCLUDE GUARDS HERE

#ifndef GUBG_MODULE
#error GUBG_MODULE should be defined by now
#endif

#include "gubg/logging/Framework.hpp"
#include "gubg/Macro.hpp"
#include <iostream>
#include <sstream>

#define L_L__(prefix, msg) \
    if (l_gubg_logging_scope_.verbose()) \
    { \
        std::ostringstream l_gubg_log_message_oss_; \
        l_gubg_log_message_oss_ << l_gubg_logging_scope_.indent() << prefix << msg << std::endl; \
        gubg::logging::Output::write(l_gubg_log_message_oss_.str()); \
    }
#define L_S__(tag_str, msg, verbose) \
    gubg::logging::Scope l_gubg_logging_scope_(GUBG_MODULE, tag_str, verbose); \
    L_L__(">>", msg)

#define S()   L_S__(__FUNCTION__, "", true)
#define S_()  L_S__(__FUNCTION__, "", false)
#define SS()  L_S__(__FUNCTION__, "", true)
#define SS_() L_S__(__FUNCTION__, "", false)

#define L(msg)  do {                          L_L__("  ", msg); } while(false)
#define L_(msg) do {                          L_L__("  ", msg); } while(false)
#define LL(msg) do { L_S__(__FUNCTION__, ""); L_L__("**", msg); } while(false)
