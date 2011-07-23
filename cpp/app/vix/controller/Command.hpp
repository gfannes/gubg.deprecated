#ifndef vix_controller_Command_hpp
#define vix_controller_Command_hpp

#include "model/Selection.hpp"
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
    class Commander;

    namespace command
    {
        //Navigation-related commands
        class Open: public ICommand
        {
            public:
                Open(Commander &commander, model::Action action):
                    commander_(commander),
                    action_(action){}
                virtual bool execute();
                virtual std::string description() const {return "Open this location.";}
            private:
                Commander &commander_;
                model::Action action_;
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
                virtual std::string description() const {return "New tab";}
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

        //Application control
        class Quit: public ICommand
        {
            public:
                virtual bool execute();
                virtual std::string description() const {return "Quit";}
        };
    }
}

#endif