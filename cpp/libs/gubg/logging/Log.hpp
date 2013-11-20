#ifndef HEADER_gubg_logging_Log_hpp_ALREADY_INCLUDED
#define HEADER_gubg_logging_Log_hpp_ALREADY_INCLUDED

#include "gubg/Macro.hpp"
#include "gubg/logging/Framework.hpp"
#include <string>
#include <sstream>

//This header file provides logging functionality:
// * #define GUBG_LOG to enable logging before including this header
// * LOG_S(tag) creates a new log scope
//    * Not wrapped in {}
// * LOG_S(tag, msg) creates a new log scope and logs a message
//    * Not wrapped in {}
// * LOG_M(msg) logs a message is a previously opened scope
//    * Wrapped in {}

//Support for log levels is also present
// * #define LOG_LEVEL [Fubar|Fatal|Error|Warning|Info|Debug] before including this header
// * Use LOG_S_(level, tag, [msg]) to open a scope in some level. The scope is alsways created, but the message might not be logged depending on the log level
// * Use LOG_M_(level, msg) to log a message at a certain level

#ifdef LOG_LEVEL
#ifndef GUBG_LOG
#define GUBG_LOG
#endif
#else
#define LOG_LEVEL Error
#endif
#define LOG_S__2(level, tag) LOG_S_ ## level(tag)
#define LOG_S__3(level, tag, msg) LOG_SM_ ## level(tag, msg)
#define LOG_S__MACRO_CHOOSER(...) GUBG_GET_4TH_ARG(__VA_ARGS__, LOG_S__3,LOG_S__2)
#define LOG_S_(...) LOG_S__MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define LOG_M_(level, msg) LOG_M_ ## level(msg)

#ifndef GUBG_MODULE
#error You have to specify GUBG_MODULE as a string
#endif
namespace
{
    const char *l_gubg_logging_module__(GUBG_MODULE);
}

#ifdef GUBG_LOG
#define L_LOG_PRIM(prefix, msg) \
    { \
        std::ostringstream l_gubg_logging_message_oss_; \
        l_gubg_logging_message_oss_ << l_gubg_logging_scope_.indent() << prefix << msg << std::endl; \
        gubg::logging::Output::write(l_gubg_logging_message_oss_.str()); \
    }
#define L_LOG_SCOPE(tag, msg, doLog) \
    const bool l_gubg_logging_verbose_ = doLog; \
    gubg::logging::Scope l_gubg_logging_scope_(l_gubg_logging_module__, #tag, l_gubg_logging_verbose_); \
    if (l_gubg_logging_verbose_) L_LOG_PRIM(">>", msg)
#define LOG_M(msg) do { if (l_gubg_logging_verbose_) L_LOG_PRIM("  ", msg); } while(false)

#else
#define L_LOG_SCOPE(tag, msg, doLog)
#define LOG_M(msg)
#endif

#define LOG_S_1(tag) L_LOG_SCOPE(tag, "", true)
#define LOG_S_2(tag, msg) L_LOG_SCOPE(tag, msg, true)
#define LOG_SQ_1(tag) L_LOG_SCOPE(tag, "", false)
#define LOG_SQ_2(tag, msg) L_LOG_SCOPE(tag, msg, false)
#define LOG_S_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__, LOG_S_2,LOG_S_1)
#define LOG_S(...) LOG_S_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define LOG_SQ_MACRO_CHOOSER(...) GUBG_GET_3TH_ARG(__VA_ARGS__, LOG_SQ_2,LOG_SQ_1)
#define LOG_SQ(...) LOG_SQ_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define GUBG_LOG_LEVEL_Debug   0
#define GUBG_LOG_LEVEL_Info    1
#define GUBG_LOG_LEVEL_Warning 2
#define GUBG_LOG_LEVEL_Error   3
#define GUBG_LOG_LEVEL_Fatal   4
#define GUBG_LOG_LEVEL_Fubar   5

#define LOG_S_Fubar(tag) LOG_S(tag)
#define LOG_SM_Fubar(tag, msg) LOG_S(tag, "<Fubar>" << msg)
#define LOG_M_Fubar(msg) LOG_M("<Fubar>" << msg)
#define LOG_S_Fatal(tag) LOG_S(tag)
#define LOG_SM_Fatal(tag, msg) LOG_S(tag, "<Fatal>" << msg)
#define LOG_M_Fatal(msg) LOG_M("<Fatal>" << msg)
#define LOG_S_Error(tag) LOG_S(tag)
#define LOG_SM_Error(tag, msg) LOG_S(tag, "<Error>" << msg)
#define LOG_M_Error(msg) LOG_M("<Error>" << msg)
#define LOG_S_Warning(tag) LOG_S(tag)
#define LOG_SM_Warning(tag, msg) LOG_S(tag, "<Warning>" << msg)
#define LOG_M_Warning(msg) LOG_M("<Warning>" << msg)
#define LOG_S_Info(tag) LOG_S(tag)
#define LOG_SM_Info(tag, msg) LOG_S(tag, "<Info>" << msg)
#define LOG_M_Info(msg) LOG_M("<Info>" << msg)
#define LOG_S_Debug(tag) LOG_S(tag)
#define LOG_SM_Debug(tag, msg) LOG_S(tag, "<Debug>" << msg)
#define LOG_M_Debug(msg) LOG_M("<Debug>" << msg)

#define L_LOG_LEVEL__(level) GUBG_LOG_LEVEL_ ## level
#define L_LOG_LEVEL_(level) L_LOG_LEVEL__(level)
#define L_LOG_LEVEL L_LOG_LEVEL_(LOG_LEVEL)

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fubar
#undef LOG_S_Fatal
#undef LOG_SM_Fatal
#define LOG_S_Fatal(tag) LOG_SQ_1(tag)
#define LOG_SM_Fatal(tag, msg) LOG_SQ_1(tag)
#undef LOG_M_Fatal
#define LOG_M_Fatal(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Fatal
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fatal
#undef LOG_S_Error
#undef LOG_SM_Error
#define LOG_S_Error(tag) LOG_SQ_1(tag)
#define LOG_SM_Error(tag, msg) LOG_SQ_1(tag)
#undef LOG_M_Error
#define LOG_M_Error(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Error
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Error
#undef LOG_S_Warning
#undef LOG_SM_Warning
#define LOG_S_Warning(tag) LOG_SQ_1(tag)
#define LOG_SM_Warning(tag, msg) LOG_SQ_1(tag)
#undef LOG_M_Warning
#define LOG_M_Warning(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Warning
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Warning
#undef LOG_S_Info
#undef LOG_SM_Info
#define LOG_S_Info(tag) LOG_SQ_1(tag)
#define LOG_SM_Info(tag, msg) LOG_SQ_1(tag)
#undef LOG_M_Info
#define LOG_M_Info(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Info
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Info
#undef LOG_S_Debug
#undef LOG_SM_Debug
#define LOG_S_Debug(tag) LOG_SQ_1(tag)
#define LOG_SM_Debug(tag, msg) LOG_SQ_1(tag)
#undef LOG_M_Debug
#define LOG_M_Debug(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Debug
#endif

#if L_LOG_LEVEL != GUBG_LOG_LEVEL_Debug
#error You can only use LOG_LEVEL Fubar, Fatal, Error, Warning, Info or Debug
#endif

#endif
