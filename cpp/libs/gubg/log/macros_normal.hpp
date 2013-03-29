//NO INCLUDE GUARDS HERE

#include "gubg/logging/Framework.hpp"
#include "gubg/macro.hpp"
#include <iostream>

#define L_L__(prefix, msg) \
    { \
        std::ostringstream l_gubg_logging_message_oss_; \
        l_gubg_logging_message_oss_ << l_gubg_logging_scope_.indent() << prefix << msg << std::endl; \
        gubg::logging::Output::write(l_gubg_logging_message_oss_.str()); \
    }
#define L_S__(tag_str, msg, doLog) \
    const bool l_gubg_logging_verbose_ = doLog; \
    gubg::logging::Scope l_gubg_logging_scope_(GUBG_MODULE, tag_str, l_gubg_logging_verbose_); \
    if (l_gubg_logging_verbose_) L_L__(">>", msg)

#define S()      L_S__(__FUNCTION__, "", false)
#define S_()     L_S__(__FUNCTION__, "", true)
#define SS(...)  L_S__(__FUNCTION__, STREAM(__VA_ARGS__), false)
#define SS_(...) L_S__(__FUNCTION__, STREAM(__VA_ARGS__), true)

#define L(msg)  do { if (l_gubg_logging_verbose_)    L_L__("  ", msg); } while(false)
#define L_(msg) do {                                 L_L__("  ", msg); } while(false)
#define LL(msg) do { L_S__(__FUNCTION__, "", false); L_L__("##", msg); } while(false)
