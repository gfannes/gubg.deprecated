#ifndef HEADER_gubg_logging_Framework_hpp_ALREADY_INCLUDED
#define HEADER_gubg_logging_Framework_hpp_ALREADY_INCLUDED

#include "boost/thread/thread.hpp"
#include "boost/thread/tss.hpp"
#include "boost/algorithm/string/join.hpp"
#include <iostream>
#include <memory>

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
            Name(const char *m, const char *n):
                module(m), name(n){}
            const char *module;
            const char *name;
        };
        struct Scope
        {
            typedef std::vector<Name> NameStack;
            Scope(const char *module, const char *name, bool verboseDtor):
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

            const char *name() const {return nameStack_->back().name;}
            const char *module() const {return nameStack_->back().module;}
            std::string indent()
            {
                if (!indent_)
                {
                    std::ostringstream oss;
                    oss << threadId_;
                    const char *currentModule = 0;
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
