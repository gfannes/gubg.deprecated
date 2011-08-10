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

    class Commander: public gubg::MetaStateMachine<Event>
    {
        public:
            typedef gubg::MetaStateMachine<Event> Base;
            typedef boost::signals2::signal<void (int, std::string *)> UpdateSignal;
            Commander(model::Selections &);

            boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

            void add(ICommand::Ptr);
            void add(char ch);
            void clear();
            std::string getText() const;
            bool isFilter() const;

            enum class Key {Enter, Arrow};
            void activate(Key);

            void changeTab(int ix);

        protected: 
            virtual bool processEventLocally(Event);

        private:
            void filterChanged_(std::string *);
            void contentChanged_(std::string *);
            void commandChanged_(std::string *);
            void update_();
            UpdateSignal updated_;

            void executeCommands_();

            friend class vix::command::Open;
            friend class vix::command::NewTab;
            model::Selections &selections_;
            Instruction getInstruction() const {return Instruction(text_);}
            std::string text_;

            typedef std::deque<ICommand::Ptr> Commands;
            Commands pendingCommands_;
            Commands executedCommands_;

            //The submachines
            std::shared_ptr<FilterStateMachine> filter_;
            std::shared_ptr<ContentStateMachine> content_;
            std::shared_ptr<CommandStateMachine> command_;
    };
}

#endif
