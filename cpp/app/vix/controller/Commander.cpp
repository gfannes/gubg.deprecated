#include "controller/Commander.hpp"
#include "controller/Command.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
using namespace std;

namespace vix
{
    Commander::Commander(model::Selections &selections):
        selections_(selections)
    {
        MetaState::filter = &filter_;
        MetaState::content = &content_;
        MetaState::command = &command_;
        connect_(Control::Filter,  boost::bind(&Commander::filterChanged_, this, _1));
        connect_(Control::Content, boost::bind(&Commander::contentChanged_, this, _1));
        connect_(Control::Command, boost::bind(&Commander::commandChanged_, this, _1));
        changeState(Control::Filter);
    }

    boost::signals2::connection Commander::connect(const UpdateSignal::slot_type &subscriber)
    {
        return updated_.connect(subscriber);
    }

    void Commander::activate(Key key)
    {
        LOG_SM_(Debug, Commander::activate, "Key: " << (int)key);
        ICommand::Ptr command;
        auto instruction = getInstruction_();
        if (instruction.isValid())
        {
            LOG_M_(Debug, "A valid instruction was found");
            if (instruction.command() == "q")
            {
                if (instruction.options() == "")
                    command.reset(new command::CloseTab(*this, selections_.currentIX()));
                else if (instruction.options() == "a")
                    command.reset(new command::Quit());
            }
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
                    LOG_M_(Debug, "Opening with the enter key");
                    command.reset(new command::Open(*this, model::Action::Open));
                    break;
                case Key::Arrow:
                    LOG_M_(Debug, "Opening with the arrow key");
                    command.reset(new command::Open(*this, model::Action::Edit));
                    break;
            }
        }
        if (command)
            pendingCommands_.push_back(command);
        executeCommands_();
    }

    void Commander::clear()
    {
        LOG_SM_(Debug, Commander::clear, "");
        dispatchEvent(Special::Escape);
        update_();
    }
    void Commander::add(char ch)
    {
        dispatchEvent(ch);
        update_();
    }
    void Commander::changeTab(int ix)
    {
        selections_.setCurrent(ix);
    }

    int Commander::currentMode() const
    {
        if (state == filter)
            return 0;
        if (state == content)
            return 1;
        if (state == command)
            return 2;
        return -1;
    }

    //Private methods
    void Commander::update_()
    {
        updated_(0, &filter_.state);
        updated_(1, &content_.state);
        updated_(2, &command_.state);
    }
    void Commander::filterChanged_(const std::string *str)
    {
        selections_.current()->setFilter(*str);
        updated_(0, str);
    }
    void Commander::contentChanged_(const std::string *str)
    {
        updated_(1, str);
    }
    void Commander::commandChanged_(const std::string *str)
    {
        updated_(2, str);
    }
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
    Instruction Commander::getInstruction_() const
    {
        LOG_SM_(Debug, getInstruction, "command_.state: " << command_.state);
        return Instruction(command_.state);
    }

    void Commander::connect_(Control control, const vix::EditableString::Slot &subscriber)
    {
        switch (control)
        {
            case Control::Filter:  filter_.connect(subscriber);  break;
            case Control::Content: content_.connect(subscriber); break;
            case Control::Command: command_.connect(subscriber); break;
        }
    }
}
