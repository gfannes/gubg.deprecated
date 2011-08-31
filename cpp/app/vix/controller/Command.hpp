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
        class ToggleRecursiveMode: public ICommand
        {
            public:
                ToggleRecursiveMode(Commander &commander):
                    commander_(commander){}
                virtual bool execute();
                virtual std::string description() const {return "Toggle recursive mode.";}
            private:
                Commander &commander_;
        };

        //Tab-related commands
        class NewTab: public ICommand
        {
            public:
                NewTab(Commander &, const std::string &name);
                virtual bool execute();
                virtual std::string description() const {return "New tab";}
            private:
                Commander &commander_;
                const std::string name_;
        };
        class CloseTab: public ICommand
        {
            public:
                CloseTab(Commander &, int ix);
                virtual bool execute();
                virtual std::string description() const {return "Close tab";}
            private:
                Commander &commander_;
                const int ix_;
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
