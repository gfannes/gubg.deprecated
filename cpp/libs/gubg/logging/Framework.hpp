#ifndef HEADER_gubg_logging_Framework_hpp_ALREADY_INCLUDED
#define HEADER_gubg_logging_Framework_hpp_ALREADY_INCLUDED

#include "gubg/Singleton.hpp"
#include "gubg/Platform.hpp"
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <vector>

namespace gubg
{
    namespace logging
    {
        struct Output
        {
            static void write(const std::string &str)
            {
                static std::mutex mutex;
                std::lock_guard<std::mutex> lock(mutex);
                std::cout << str;
            }
        };
        class Scope
        {
            public:
                Scope(const char *module, const char *name, bool verboseDtor):
                    threadId_(threadId()),
                    verboseDtor_(verboseDtor)
            {
                thread_local NameStack *nameStack = 0;
                if (!(nameStack_ = nameStack))
                {
                    std::ostringstream oss;
                    oss << "Thread " << threadId_ << " logs for the first time" << std::endl;
                    Output::write(oss.str());
                    nameStack = nameStack_ = new NameStack;
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

                bool verbose() const {return verboseDtor_;}
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

            private:
                struct Name
                {
                    const char *module;
                    const char *name;
                    Name(const char *m, const char *n): module(m), name(n){}
                };
                typedef std::vector<Name> NameStack;
                const ThreadId threadId_;
                const bool verboseDtor_;
                NameStack *nameStack_;
                std::unique_ptr<std::string> indent_;
        };
    }
}


#endif
