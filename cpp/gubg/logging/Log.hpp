#ifndef gubg_logging_Log_hpp
#define gubg_logging_Log_hpp

#include "Singleton.hpp"
#include "boost/thread/tss.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/algorithm/string/join.hpp"
#include <string>
#include <iostream>

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

namespace gubg
{
    namespace logging
    {
        struct Scope
        {
            typedef std::vector<std::string> NameStack;
            Scope(const std::string &name)
            {
                auto &nameStack = gubg::Singleton<boost::thread_specific_ptr<NameStack>>::instance();
                if (!(nameStack_ = nameStack.get()))
                    nameStack.reset(nameStack_ = new NameStack);
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
                    indent_.reset(new std::string(boost::algorithm::join(*nameStack_, "|")));
                }
                return *indent_;
            }

            NameStack *nameStack_;
            boost::scoped_ptr<std::string> indent_;
        };
    }
}

#endif
