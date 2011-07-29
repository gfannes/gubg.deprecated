#ifndef vix_controller_Commander_hpp
#define vix_controller_Commander_hpp

#include "controller/Command.hpp"
#include "controller/Instruction.hpp"
#include "model/Selection.hpp"
#include <deque>

namespace vix
{
    namespace command
    {
        class Open;
        class NewTab;
    }
    class Commander
    {
        public:
            Commander(model::Selections &selections):
                selections_(selections){}

        void add(ICommand::Ptr);
        void add(char ch){text_.push_back(ch);}
        void clear(){text_.clear();}
        std::string text() const {return text_;}
        bool isFilter() const;

        enum class Key {Enter, Arrow};
        void activate(Key);

        private:
        void executeCommands_();

        friend class vix::command::Open;
        friend class vix::command::NewTab;
        model::Selections &selections_;
        Instruction getInstruction() const {return Instruction(text_);}
        std::string text_;

        typedef std::deque<ICommand::Ptr> Commands;
        Commands pendingCommands_;
        Commands executedCommands_;
    };
}

#endif
