#ifndef vix_model_Command_hpp
#define vix_model_Command_hpp

#include "pattern/Command.hpp"
#include <string>

namespace vix
{
    class ICommand: public gubg::pattern::ICommand<bool>
    {
        public:
            typedef boost::shared_ptr<ICommand> Ptr;
            virtual std::string description() const = 0;
    };

    //Navigation-related commands
    class Open: public ICommand
    {
        public:
            virtual bool execute();
    };
    class Search: public ICommand
    {
        public:
            virtual bool execute();
    };

    //Tab-related commands
    class NewTab: public ICommand
    {
        public:
            virtual bool execute();
    };
    class CloseTab: public ICommand
    {
        public:
            virtual bool execute();
    };
    class FocusTab: public ICommand
    {
        public:
            virtual bool execute();
    };
}

#endif
