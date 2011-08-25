#ifndef vix_controller_Commander_hpp
#define vix_controller_Commander_hpp

#include "controller/Command.hpp"
#include "controller/Instruction.hpp"
#include "controller/StateMachine.hpp"
#include "model/Selection.hpp"
#include "boost/signals2.hpp"
#include <deque>
#include <string>

namespace vix
{
    namespace command
    {
        class Open;
        class NewTab;
    }

    class Commander: public MetaMachine
    {
        public:
            typedef MetaMachine Base;
            typedef boost::signals2::signal<void (int, const std::string *)> UpdateSignal;
            Commander(model::Selections &);

            boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

            void add(ICommand::Ptr);
            void add(char ch);
            void clear();

            enum class Key {Enter, Arrow};
            void activate(Key);

            void changeTab(int ix);

        private:
            void filterChanged_(const std::string *);
            void contentChanged_(const std::string *);
            void commandChanged_(const std::string *);
            void update_();
            UpdateSignal updated_;

            void executeCommands_();

            friend class vix::command::Open;
            friend class vix::command::NewTab;
            model::Selections &selections_;
            Instruction getInstruction_() const;

            typedef std::deque<ICommand::Ptr> Commands;
            Commands pendingCommands_;
            Commands executedCommands_;

            //The submachines
            void connect_(Control, const vix::EditableString::Slot &);
            FilterStateMachine filter_;
            ContentStateMachine content_;
            CommandStateMachine command_;
    };
}

#endif
