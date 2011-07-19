#ifndef gubg_pattern_Command_hpp
#define gubg_pattern_Command_hpp

#include "Exception.hpp"
#include "boost/shared_ptr.hpp"

namespace gubg
{
    namespace pattern
    {
        //Abstract command interface, to be used by the invoker
        class ICommand
        {
            public:
                typedef boost::shared_ptr<ICommand> Ptr;

                virtual ~ICommand(){}

                //This needs to be implemented
                virtual bool execute() = 0;

                //Undo is optional
                virtual bool undo()
                {
                    gubg::Exception::raise(gubg::NotImplemented("The undo()-method is not implemented for gubg::pattern::ICommand"));
                    return false;
                }
        };

        //Slightly more concrete base class which clearly states the receiver
        template <typename Receiver>
            class Command: public ICommand
        {
            public:
                Command(Receiver &receiver):
                    receiver_(receiver){}

            protected:
                Receiver &receiver_;
        };
    }
}

#endif
