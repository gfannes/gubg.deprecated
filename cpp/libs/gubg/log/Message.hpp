#ifndef HEADER_gubg_log_Message_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_Message_hpp_ALREADY_INCLUDED

#include <iostream>
#include <memory>

//The principles of this logging framework are:
//* A Module type represents how logging should happen for some module, an instance provides the necessary means
//* A Stream type represents a single destination (cout, file with some name, ...), an instance provides this actual stream
//* Logging levels should be attached to _types_, the Level traits template can help with this
//
//* A log message follows the lifetime of Message, which is typically placed on the stack
//* If Module::doLog(level) determines actual logging should happen, an instance of Module is created
//** It should have an std::ostream member called os
//** Its ctor/dtor can add a header/footer to the log message, RAII-style
//
//Most of the time, the Module class inherits from a Stream to provide it with its os member
//Also, the doLog() can be provided by inheriting from LeveledStream<Stream>, which gives you
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
                Message(const Message &) = delete;
                Message(Message &&) = delete;
                Message &operator=(const Message &) = delete;
                Message &operator=(Message &&) = delete;

                //To optimize logging, you could try to replace this with something based on placement-new
                std::unique_ptr<Module> mod_;
        };

    //Helper traits template that attaches an int value of 0 to every type by default
    template <typename T>
        struct Level
        {
            static int value;
        };
    template <typename T>
        int Level<T>::value = 0;

    //Provides a stream with the default level mechanics: the combined log and module level should at least equal the stream level
    template <typename Stream>
        struct LeveledStream: Stream
    {
        template <typename Module>
            static bool doLog(int level)
            {
                return level + Level<Module>::value >= Level<Stream>::value;
            }
    };

    //Streams
    struct Cout
    {
        std::ostream &os = std::cout;
    };

    //Some basic doLog() implementations
    struct Always
    {
        template <typename Module>
            static bool doLog(int) {return true;}
    };
    struct Never
    {
        template <typename Module>
            static bool doLog(int) {return false;}
    };
    template <int Lvl>
        struct IfAboveLevel
        {
            template <typename Module>
                static bool doLog(int lvl) {return lvl >= Lvl;}
        };

} }

#endif
