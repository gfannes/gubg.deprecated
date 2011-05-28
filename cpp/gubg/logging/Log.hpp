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

#ifdef LOG_LEVEL
#ifndef GUBG_LOG
#define GUBG_LOG
#endif
#else
#define LOG_LEVEL Error
#endif
#define LOG_S_(level, tag, msg...) LOG_S_ ## level(tag, msg...)
#define LOG_M_(level, msg) LOG_M_ ## level(msg)

#ifdef GUBG_LOG
#define LOG_S(tag, msg...) \
    gubg::logging::Scope l_gubg_logging_scope_(#tag); \
    std::cout << l_gubg_logging_scope_.indent() << ">>" << msg << std::endl
#else
#define LOG_S(tag, msg...)
#endif

#ifdef GUBG_LOG
#define LOG_M(msg) \
    std::cout << l_gubg_logging_scope_.indent() << "  " << msg << std::endl;
#else
#define LOG_M(msg)
#endif

#define GUBG_LOG_LEVEL_Debug   0
#define GUBG_LOG_LEVEL_Info    1
#define GUBG_LOG_LEVEL_Warning 2
#define GUBG_LOG_LEVEL_Error   3
#define GUBG_LOG_LEVEL_Fatal   4
#define GUBG_LOG_LEVEL_Fubar   5

#define LOG_S_Fubar(tag, msg...) LOG_S(tag, "Fubar::" << msg)
#define LOG_M_Fubar(msg) LOG_M("Fubar::" << msg)
#define LOG_S_Fatal(tag, msg...) LOG_S(tag, "Fatal::" << msg)
#define LOG_M_Fatal(msg) LOG_M("Fatal::" << msg)
#define LOG_S_Error(tag, msg...) LOG_S(tag, "Error::" << msg)
#define LOG_M_Error(msg) LOG_M("Error::" << msg)
#define LOG_S_Warning(tag, msg...) LOG_S(tag, "Warning::" << msg)
#define LOG_M_Warning(msg) LOG_M("Warning::" << msg)
#define LOG_S_Info(tag, msg...) LOG_S(tag, "Info::" << msg)
#define LOG_M_Info(msg) LOG_M("Info::" << msg)
#define LOG_S_Debug(tag, msg...) LOG_S(tag, "Debug::" << msg)
#define LOG_M_Debug(msg) LOG_M("Debug::" << msg)

#define L_LOG_LEVEL__(level) GUBG_LOG_LEVEL_ ## level
#define L_LOG_LEVEL_(level) L_LOG_LEVEL__(level)
#define L_LOG_LEVEL L_LOG_LEVEL_(LOG_LEVEL)

#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fubar
#undef LOG_S_Fatal
#define LOG_S_Fatal(tag, msg...)
#undef LOG_M_Fatal
#define LOG_M_Fatal(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Fatal
#endif
#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Fatal
#undef LOG_S_Error
#define LOG_S_Error(tag, msg...)
#undef LOG_M_Error
#define LOG_M_Error(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Error
#endif
#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Error
#undef LOG_S_Warning
#define LOG_S_Warning(tag, msg...)
#undef LOG_M_Warning
#define LOG_M_Warning(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Warning
#endif
#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Warning
#undef LOG_S_Info
#define LOG_S_Info(tag, msg...)
#undef LOG_M_Info
#define LOG_M_Info(msg)
#undef L_LOG_LEVEL
#define L_LOG_LEVEL GUBG_LOG_LEVEL_Info
#endif
#if L_LOG_LEVEL == GUBG_LOG_LEVEL_Info
#undef LOG_S_Debug
#define LOG_S_Debug(tag, msg...)
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
        struct Scope
        {
            typedef std::vector<std::string> NameStack;
            Scope(const std::string &name):
                threadId_(boost::this_thread::get_id())
            {
                auto &nameStack = gubg::Singleton<boost::thread_specific_ptr<NameStack>>::instance();
                if (!(nameStack_ = nameStack.get()))
                {
                    std::cout << "Thread " << threadId_ << " logs for the first time" << std::endl;
                    nameStack.reset(nameStack_ = new NameStack);
                }
                nameStack_->push_back(name);
            }
            ~Scope()
            {
                std::cout << indent() << "<<" << std::endl;
                nameStack_->pop_back();
            }

            std::string name() const {return nameStack_->back();}
            std::string indent()
            {
                if (!indent_)
                {
                    std::ostringstream oss;
                    oss << threadId_ << "|" << boost::algorithm::join(*nameStack_, "|");
                    indent_.reset(new std::string(oss.str()));
                }
                return *indent_;
            }

            boost::thread::id threadId_;
            NameStack *nameStack_;
            boost::scoped_ptr<std::string> indent_;
        };
    }
}

#endif
