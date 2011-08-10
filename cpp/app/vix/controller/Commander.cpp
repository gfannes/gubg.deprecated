#include "controller/Commander.hpp"
#include "controller/Command.hpp"
using namespace std;

namespace vix
{
    Commander::Commander(model::Selections &selections):
        Base(std::shared_ptr<StateMachine>()),
        selections_(selections),
        filter_(new FilterStateMachine),
        content_(new ContentStateMachine),
        command_(new CommandStateMachine)
    {
        MetaStateMachine::connect(Control::Filter, boost::bind(&Commander::filterChanged_, this, _1));
        MetaStateMachine::connect(Control::Content, boost::bind(&Commander::contentChanged_, this, _1));
        MetaStateMachine::connect(Control::Command, boost::bind(&Commander::commandChanged_, this, _1));
        changeState(filter_);
    }

    boost::signals2::connection Commander::connect(const UpdateSignal::slot_type &subscriber)
    {
        return updated_.connect(subscriber);
    }

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

    void Commander::clear()
    {
        processEvent(Event(Event::Escape));
        update_();
    }
    void Commander::add(char ch)
    {
        processEvent(Event(ch));
        update_();
    }
    void Commander::changeTab(int ix)
    {
        selections_.setCurrent(ix);
        text_ = selections_.current()->getFilter();
    }
    string Commander::getText() const
    {
        return text_;
    }

    //Private methods
    void Commander::update_()
    {
        string str(getFilter());
        updated_(0, &str);
        str = getContent();
        updated_(1, &str);
        str = getCommand();
        updated_(2, &str);
    }
    void Commander::filterChanged_(std::string *str)
    {
        selections_.current()->setFilter(*str);
        updated_(0, str);
    }
    void Commander::contentChanged_(std::string *str)
    {
        updated_(1, str);
    }
    void Commander::commandChanged_(std::string *str)
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

    bool Commander::processEventLocally(Event event)
    {
        switch (event.type)
        {
            case Event::CharT:
                {
                    switch (event.value.ch)
                    {
                        case ':':
                            changeState(command_);
                            break;
                        case '/':
                            changeState(content_);
                            break;
                    }
                }
                break;
            case Event::SpecialT:
                {
                    switch (event.value.special)
                    {
                        case Event::Escape:
                            changeState(filter_);
                            break;
                    }
                }
                break;
        }
        return true;
    }
    string MetaStateMachine::getFilter() const
    {
        return filter_->state();
    }
    string MetaStateMachine::getContent() const
    {
        return content_->state();
    }
    string MetaStateMachine::getCommand() const
    {
        return command_->state();
    }

    void MetaStateMachine::connect(Control control, const vix::StateMachine::Slot &subscriber)
    {
        switch (control)
        {
            case Control::Filter:
                filter_->connect(subscriber);
                break;
            case Control::Content:
                content_->connect(subscriber);
                break;
            case Control::Command:
                command_->connect(subscriber);
                break;
        }
    }
}
