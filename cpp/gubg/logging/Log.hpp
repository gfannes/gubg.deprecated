#ifndef gubg_logging_Log_hpp
#define gubg_logging_Log_hpp

#include "Singleton.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/tss.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/algorithm/string/join.hpp"
#include <string>
#include <iostream>
#include <sstream>

//This header file provides logging functionality:
// * #define GUBG_LOG to enable logging before including this header
// * LOG_S(tag) creates a new log scope
//    * Not wrapped in {}
// * LOG_SM(tag, msg) creates a new log scope and logs a message
//    * Not wrapped in {}
// * LOG_M(msg) logs a message is a previously opened scope
//    * Wrapped in {}

//Support for log levels is also present
// * #define LOG_LEVEL [Fubar|Fatal|Error|Warning|Info|Debug] before including this header
// * Use LOG_S[M]_(level, tag, [msg]) to open a scope in some level. The scope is alsways created, but the message might not be logged depending on the log level
// * Use LOG_M_(level, msg) to log a message at a certain level

#ifdef LOG_LEVEL
#ifndef GUBG_LOG
#define GUBG_LOG
#endif
#else
#define LOG_LEVEL Error
#endif
#define LOG_S_(level, tag) LOG_S_ ## level(tag)
#define LOG_SM_(level, tag, msg) LOG_SM_ ## level(tag, msg)
#define LOG_M_(level, msg) LOG_M_ ## level(msg)

#ifdef GUBG_LOG
#define LOG_S(tag) \
    gubg::logging::Scope l_gubg_logging_scope_(#tag, true); \
    std::ostringstream l_gubg_logging_scope_oss_; \
    l_gubg_logging_scope_oss_ << l_gubg_logging_scope_.indent() << ">>" << std::endl; \
    gubg::logging::Output::write(l_gubg_logging_scope_oss_.str());
#define LOG_SM(tag, msg) \
    gubg::logging::Scope l_gubg_logging_scope_(#tag, true); \
    std::ostringstream l_gubg_logging_scope_oss_; \
    l_gubg_logging_scope_oss_ << l_gubg_logging_scope_.indent() << ">>" << msg << std::endl; \
    gubg::logging::Output::write(l_gubg_logging_scope_oss_.str());
#define LOG_S_SILENT(tag) \
    gubg::logging::Scope l_gubg_logging_scope_(#tag, false);
#define LOG_M(msg) \
    { \
        std::ostringstream l_gubg_logging_message_oss_; \
        l_gubg_logging_message_oss_ << l_gubg_logging_scope_.indent() << "  " << msg << std::endl; \
        gubg::logging::Output::write(l_gubg_logging_message_oss_.str()); \
    }
#else
#define LOG_S(tag)
#define LOG_SM(tag, msg)
#define LOG_S_SILENT(tag)
#define LOG_M(msg)
#endif

#define GUBG_LOG_LEVEL_Debug   0
#define GUBG_LOG_LEVEL_Info    1
#define GUBG_LOG_LEVEL_Warning 2
#define GUBG_LOG_LEVEL_Error   3
#define GUBG_LOG_LEVEL_Fatal   4
#define GUBG_LOG_LEVEL_Fubar   5

#define LOG_S_Fubar(tag) LOG_S(tag)
#define LOG_SM_Fubar(tag, msg) LOG_SM(tag, "<Fubar>" << msg)
#define LOG_M_Fubar(msg) LOG_M("<Fubar>" << msg)
#define LOG_S_Fatal(tag) LOG_S(tag)
#define LOG_SM_Fatal(tag, msg) LOG_SM(tag, "<Fatal>" << msg)
#define LOG_M_Fatal(msg) LOG_M("<Fatal>" << msg)
#define LOG_S_Error(tag) LOG_S(tag)
#define LOG_SM_Error(tag, msg) LOG_SM(tag, "<Error>" << msg)
#define LOG_M_Error(msg) LOG_M("<Error>" << msg)
#define LOG_S_Warning(tag) LOG_S(tag)
#define LOG_SM_Warning(tag, msg) LOG_SM(tag, "<Warning>" << msg)
#define LOG_M_Warning(msg) LOG_M("<Warning>" << msg)
#define LOG_S_Info(tag) LOG_S(tag)
#define LOG_SM_Info(tag, msg) LOG_SM(tag, "<Info>" << msg)
#define LOG_M_Info(msg) LOG_M("<Info>" << msg)
#define LOG_S_Debug(tag) LOG_S(tag)
#define LOG_SM_Debug(tag, msg) LOG_SM(tag, "<Debug>" << msg)
#define LOG_M_Debug(msg) LOG_M("<Debug>" << msg)

#define L_LOG_LEVEL__(level) GUBG_LOG_LEVEL_ ## level
#define L_LOG_LEVEL_(level) L_LOG_LEVEL__(level)
#define L_LOG_LEVEL L_LOG_LEVEL_(LOG_LEVEL)

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fubar
#undef LOG_S_Fatal
#undef LOG_SM_Fatal
#define LOG_S_Fatal(tag) LOG_S_SILENT(tag)
#define LOG_SM_Fatal(tag, msg) LOG_S_SILENT(tag)
#undef LOG_M_Fatal
#define LOG_M_Fatal(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Fatal
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fatal
#undef LOG_S_Error
#undef LOG_SM_Error
#define LOG_S_Error(tag) LOG_S_SILENT(tag)
#define LOG_SM_Error(tag, msg) LOG_S_SILENT(tag)
#undef LOG_M_Error
#define LOG_M_Error(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Error
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Error
#undef LOG_S_Warning
#undef LOG_SM_Warning
#define LOG_S_Warning(tag) LOG_S_SILENT(tag)
#define LOG_SM_Warning(tag, msg) LOG_S_SILENT(tag)
#undef LOG_M_Warning
#define LOG_M_Warning(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Warning
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Warning
#undef LOG_S_Info
#undef LOG_SM_Info
#define LOG_S_Info(tag) LOG_S_SILENT(tag)
#define LOG_SM_Info(tag, msg) LOG_S_SILENT(tag)
#undef LOG_M_Info
#define LOG_M_Info(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Info
#endif

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Info
#undef LOG_S_Debug
#undef LOG_SM_Debug
#define LOG_S_Debug(tag) LOG_S_SILENT(tag)
#define LOG_SM_Debug(tag, msg) LOG_S_SILENT(tag)
#undef LOG_M_Debug
#define LOG_M_Debug(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Debug
#endif

#if L_LOG_LEVEL != GUBG_LOG_LEVEL_Debug
#error You can only use LOG_LEVEL Fubar, Fatal, Error, Warning, Info or Debug
#endif

namespace gubg
{
    namespace logging
    {
        struct Output
        {
            static void write(const std::string &str)
            {
                static boost::mutex mutex;
                boost::mutex::scoped_lock lock(mutex);
                std::cout << str;
            }
        };
        struct Scope
        {
            typedef std::vector<std::string> NameStack;
            Scope(const std::string &name, bool verboseDtor):
                threadId_(boost::this_thread::get_id()),
                verboseDtor_(verboseDtor)
            {
                auto &nameStack = gubg::Singleton<boost::thread_specific_ptr<NameStack>>::instance();
                if (!(nameStack_ = nameStack.get()))
                {
                    std::ostringstream oss;
                    oss << "Thread " << threadId_ << " logs for the first time" << std::endl;
                    Output::write(oss.str());
                    nameStack.reset(nameStack_ = new NameStack);
                }
                nameStack_->push_back(name);
            }
            ~Scope()
            {
                if (verboseDtor_)
                {
                    std::ostringstream oss;
                    oss << indent() << "<<" << std::endl;
                    Output::write(oss.str());
                }
                nameStack_->pop_back();
            }

            std::string name() const {return nameStack_->back();}
            std::string indent()
            {
                if (!indent_)
                {
                    std::ostringstream oss;
                    oss << threadId_ << "::" << boost::algorithm::join(*nameStack_, "::");
                    indent_.reset(new std::string(oss.str()));
                }
                return *indent_;
            }

            boost::thread::id threadId_;
            bool verboseDtor_;
            NameStack *nameStack_;
            boost::scoped_ptr<std::string> indent_;
        };
    }
}

#endif
