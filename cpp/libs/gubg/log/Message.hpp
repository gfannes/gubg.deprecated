#ifndef HEADER_gubg_log_Message_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_Message_hpp_ALREADY_INCLUDED

#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include "gubg/log/Stream.hpp"
#include "gubg/log/Level.hpp"

//The principles of this logging framework are:
//* A log message is represented by the Message template, the Module template parameters provides
//  it with the necessary context info. I provides the necessary logic to determine _if_ logging
//  should happen, and should be short-lived, hence its typical placement on the stack
//* A _Module type_ represents how logging should happen for some module
//* A _Module instance_ is only created when actual logging should happen and follows the life time of Message
//  This instance should have a streamable _os_ member, and can provide Module-level specifics like a header/footer
//* A _Stream type_ represents a single destination (cout, file with some name, ...)
//* A _Stream instance_ provides this actual stream and can provide extra things like locking
//  since it too follows the life time of a Message
//* Logging levels should be attached to _types_, the Level traits template can help with this
//
//Most of the time, the Module class inherits from a Stream to provide it with its os member
//Also, the doLog() static method can be provided by inheriting from LeveledStream<Stream>, which gives you
//the flexibilicy to use the Level<>s attached to Module and Stream, in combination with the
//level of the Message itself

namespace gubg { namespace log {

    //This is a log message, you can create it with a level (the higher, the more important)
    //If it is not important enough (see Module::doLog()), all logging is lost
    //The template parameter Module is instantiated if logging should happen, and it should
    //create its output stream "os" somehow, e.g., by inheritance from a Stream (e.g., Cout)
    //doLog can be provided using LeveledStream
    template <typename Module>
        class Message
        {
            public:
                Message()
                {
                    //We pass the Module type info to doLog() again, this allows doLog() to be
                    //implemented in same base class of Module, but still use the most specific
                    //type info. Note the usage of "template" to indicate doLog() is a template
                    if (Module::template doLog<Module>(0))
                        //This message is important enough, we create the log Module
                        mod_.reset(new Module);
                }
                Message(int level)
                {
                    if (Module::template doLog<Module>(level))
                        //This message is important enough, we create the log Module
                        mod_.reset(new Module);
                }

                //Generic streaming
                template <typename T>
                    const Message &operator<<(const T &t) const
                    {
                        if (mod_)
                            mod_->os << t;
                        return *this;
                    }
                //Necessary to be able to stream std::endl, which is a template
                typedef std::ostream& (Manipulator)(std::ostream&);
                const Message &operator<<(const Manipulator &t) const
                {
                    if (mod_)
                        mod_->os << t;
                    return *this;
                }

            private:
                //An instance of Message is meant to have typical stack life time, hence no copying allowed
                Message(const Message &) = delete;
                Message(Message &&) = delete;
                Message &operator=(const Message &) = delete;
                Message &operator=(Message &&) = delete;

                //To optimize logging, you could try to replace this with something based on placement-new
                std::unique_ptr<Module> mod_;
        };

} }

#endif
