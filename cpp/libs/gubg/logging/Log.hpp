#ifndef gubg_logging_Log_hpp
#define gubg_logging_Log_hpp

#include "gubg/Singleton.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/tss.hpp"
#include "boost/algorithm/string/join.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

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
#define GET_4TH_ARG(_1,_2,_3,_4,...) _4
#define LOG_S__MACRO_CHOOSER(...) GET_4TH_ARG(__VA_ARGS__, LOG_S__3,LOG_S__2)
#define LOG_S_(...) LOG_S__MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define LOG_M_(level, msg) LOG_M_ ## level(msg)

#ifndef GUBG_MODULE
#define GUBG_MODULE "?"
#endif
namespace
{
    const std::string l_gubg_logging_module__(GUBG_MODULE);
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
#define GET_3TH_ARG(_1,_2,_3,...) _3
#define LOG_S_MACRO_CHOOSER(...) GET_3TH_ARG(__VA_ARGS__, LOG_S_2,LOG_S_1)
#define LOG_S(...) LOG_S_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define LOG_SQ_MACRO_CHOOSER(...) GET_3TH_ARG(__VA_ARGS__, LOG_SQ_2,LOG_SQ_1)
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

//Streaming of variables
#define STREAM_FORMAT_A(var) "{" #var ": " << var <<
#define STREAM_FORMAT_B(var) ", " #var ": " << var <<
#define STREAM_1(_1) STREAM_FORMAT_A(_1)
#define STREAM_2(_1,_2                         )  STREAM_1(_1                        ) STREAM_FORMAT_B(_2)
#define STREAM_3(_1,_2,_3                      )  STREAM_2(_1,_2                     ) STREAM_FORMAT_B(_3)
#define STREAM_4(_1,_2,_3,_4                   )  STREAM_3(_1,_2,_3                  ) STREAM_FORMAT_B(_4)
#define STREAM_5(_1,_2,_3,_4,_5                )  STREAM_4(_1,_2,_3,_4               ) STREAM_FORMAT_B(_5)
#define STREAM_6(_1,_2,_3,_4,_5,_6             )  STREAM_5(_1,_2,_3,_4,_5            ) STREAM_FORMAT_B(_6)
#define STREAM_7(_1,_2,_3,_4,_5,_6,_7          )  STREAM_6(_1,_2,_3,_4,_5,_6         ) STREAM_FORMAT_B(_7)
#define STREAM_8(_1,_2,_3,_4,_5,_6,_7,_8       )  STREAM_7(_1,_2,_3,_4,_5,_6,_7      ) STREAM_FORMAT_B(_8)
#define STREAM_9(_1,_2,_3,_4,_5,_6,_7,_8,_9    )  STREAM_8(_1,_2,_3,_4,_5,_6,_7,_8   ) STREAM_FORMAT_B(_9)
#define STREAM_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) STREAM_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) STREAM_FORMAT_B(_10)
#define GET_11TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,...) _11
#define STREAM_MACRO_CHOOSER(...) GET_11TH_ARG(__VA_ARGS__, STREAM_10,STREAM_9,STREAM_8,STREAM_7,STREAM_6,STREAM_5,STREAM_4,STREAM_3,STREAM_2,STREAM_1)
#define STREAM(...) STREAM_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) "}"

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
        struct Name
        {
            Name(const std::string &m, const std::string &n):
                module(m), name(n){}
            std::string module;
            std::string name;
        };
        struct Scope
        {
            typedef std::vector<Name> NameStack;
            Scope(const std::string &module, const std::string &name, bool verboseDtor):
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
                nameStack_->push_back(Name(module, name));
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

            std::string name() const {return nameStack_->back().name;}
            std::string module() const {return nameStack_->back().module;}
            std::string indent()
            {
                if (!indent_)
                {
                    std::ostringstream oss;
                    oss << threadId_;
                    std::string currentModule;
                    for (NameStack::const_iterator name = nameStack_->begin(); name != nameStack_->end(); ++name)
                    {
                        if (currentModule != name->module)
                        {
                            currentModule = name->module;
                            oss << "->(" << name->module << ")";
                        }
                        oss << "->" << name->name;
                    }
                    indent_.reset(new std::string(oss.str()));
                }
                return *indent_;
            }

            boost::thread::id threadId_;
            bool verboseDtor_;
            NameStack *nameStack_;
	    std::unique_ptr<std::string> indent_;
        };
    }
}

#endif
