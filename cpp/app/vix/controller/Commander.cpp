#include "controller/Commander.hpp"
#include "controller/Command.hpp"

namespace vix
{
    void Commander::activate(Key key)
    {
        ICommand::Ptr command;
        if (isCommand())
        {
            auto str = getCommand();
            if (str == "qa")
                command.reset(new command::Quit());
            else if (str == "t")
                command.reset(new command::NewTab());
            clear();
        }
        else
        {
            switch (key)
            {
                case Key::Enter:
                    command.reset(new command::Open(*this, model::Action::Open));
                    break;
                case Key::Arrow:
                    command.reset(new command::Open(*this, model::Action::Edit));
                    break;
            }
        }
        if (command)
            pendingCommands_.push_back(command);
        executeCommands_();
    }

    //Private methods
    void Commander::executeCommands_()
    {
        while (!pendingCommands_.empty())
        {
            auto command = pendingCommands_.front();
            pendingCommands_.pop_front();
            command->execute();
            executedCommands_.push_back(command);
        }
    }
}
