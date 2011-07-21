#ifndef gubg_pattern_Command_hpp
#define gubg_pattern_Command_hpp

#include "Exception.hpp"
#include "boost/shared_ptr.hpp"

namespace gubg
{
    namespace pattern
    {
        //Abstract command interface, to be used by the invoker
        template <typename ReturnV>
        class ICommand
        {
            public:
                typedef boost::shared_ptr<ICommand> Ptr;
                typedef ReturnV ReturnValue;

                virtual ~ICommand(){}

                //This needs to be implemented
                virtual ReturnValue execute() = 0;

                //Undo is optional
                virtual ReturnValue undo()
                {
                    gubg::Exception::raise(gubg::NotImplemented("The undo()-method is not implemented for gubg::pattern::ICommand"));
                    return ReturnValue();
                }
        };

        //Slightly more concrete base class which clearly states the receiver
        template <typename Interface, typename Receiver>
            class Command: public Interface
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
