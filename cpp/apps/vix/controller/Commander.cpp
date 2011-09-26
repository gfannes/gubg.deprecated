#include "vix/controller/Commander.hpp"
#include "vix/controller/Command.hpp"
#define GUBG_MODULE "Commander"
#define LOG_LEVEL Debug
#include "gubg/logging/Log.hpp"
using namespace std;

namespace vix
{
    namespace controller
    {
        Commander::Commander(model::Selections &selections):
            selections_(selections)
        {
            MetaState::nameFilter = &nameFilter_;
            MetaState::contentFilter = &contentFilter_;
            MetaState::command = &command_;
            connect_(Control::NameFilter,  boost::bind(&Commander::nameFilterChanged_, this, _1));
            connect_(Control::ContentFilter, boost::bind(&Commander::contentFilterChanged_, this, _1));
            connect_(Control::Command, boost::bind(&Commander::commandChanged_, this, _1));
            changeState(Control::NameFilter);
        }

        boost::signals2::connection Commander::connect(const UpdateSignal::slot_type &subscriber)
        {
            return updated_.connect(subscriber);
        }

        void Commander::activate(Special key)
        {
            LOG_SM_(Debug, activate, "Key: " << (int)key);
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
                        path = selections_.current()->path()->path();
                    command.reset(new command::NewTab(*this, path));
                }
                else if (instruction.command() == "r")
                {
                    command.reset(new command::ToggleRecursiveMode(*this));
                }
                clear();
            }
            else
            {
                switch (key)
                {
                    case Special::Enter:
                        LOG_M_(Debug, "Opening with the enter key");
                        command.reset(new command::Open(*this, model::Action::Open));
                        break;
                    case Special::Right:
                        LOG_M_(Debug, "Opening with the arrow key");
                        command.reset(new command::Open(*this, model::Action::Edit));
                        break;
                }
            }
            if (command)
                pendingCommands_.push_back(command);
            executeCommands_();
        }
        void Commander::move(Special key)
        {
            LOG_SM_(Debug, move, "");
            model::Selection *selection = selections_.current();
            if (!selection)
            {
                LOG_M_(Error, "No valid selection is present");
                return;
            }

            switch (key)
            {
                case Special::Left:
                    if (auto parent = selection->path()->location())
                    {
                        clear();
                        selection->setPath(parent);
                    }
                    break;
                case Special::Up: selection->move(vix::model::Selection::Direction::Up); break;
                case Special::Down: selection->move(vix::model::Selection::Direction::Down); break;
            }
        }

        void Commander::clear()
        {
            LOG_SM_(Debug, clear, "");
            dispatchEvent(Special::Escape);
            update_();
        }
        void Commander::add(Special key)
        {
            switch (key)
            {
                case Special::Escape: clear(); break;
                case Special::Enter:
                case Special::Right: activate(key); break;
                case Special::Left:
                case Special::Up:
                case Special::Down: move(key); break;
                default: dispatchEvent(key); break;
            }
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
            if (state == nameFilter)
                return 0;
            if (state == contentFilter)
                return 1;
            if (state == command)
                return 2;
            return -1;
        }

        //Private methods
        void Commander::update_()
        {
            updated_(0, &nameFilter_.state);
            updated_(1, &contentFilter_.state);
            updated_(2, &command_.state);
        }
        void Commander::nameFilterChanged_(const std::string *str)
        {
            auto current = selections_.current();
            if (!current)
                return;
            current->setNameFilter(*str);
            updated_(0, str);
        }
        void Commander::contentFilterChanged_(const std::string *str)
        {
            auto current = selections_.current();
            if (!current)
                return;
            current->setContentFilter(*str);
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
                case Control::NameFilter:  nameFilter_.connect(subscriber);  break;
                case Control::ContentFilter: contentFilter_.connect(subscriber); break;
                case Control::Command: command_.connect(subscriber); break;
            }
        }
    }
}
