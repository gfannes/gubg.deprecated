#include "controller/Commander.hpp"
#include "controller/Command.hpp"
using namespace std;

namespace vix
{
    bool Commander::isFilter() const
    {
        return text_.empty() || text_[0] != ':';
    }

    void Commander::activate(Key key)
    {
        ICommand::Ptr command;
        auto instruction = getInstruction();
        if (instruction.isValid())
        {
            if (instruction.command() == "q")
                command.reset(new command::Quit());
            else if (instruction.command() == "t")
            {
                string path = instruction.options();
                if (path.empty() && !selections_.empty())
                    path = vix::model::Path::Unlock(selections_.current()->path())->path();
                command.reset(new command::NewTab(*this, path));
            }
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
