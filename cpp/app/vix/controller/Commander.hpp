#ifndef vix_controller_Commander_hpp
#define vix_controller_Commander_hpp

#include "controller/Command.hpp"
#include "model/Selection.hpp"
#include <deque>

namespace vix
{
    namespace command
    {
        class Open;
    }
    class Commander
    {
        public:
            Commander(model::Selection &selection):
                selection_(selection){}

        void add(ICommand::Ptr);
        void add(char ch){text_.push_back(ch);}
        void clear(){text_.clear();}
        std::string text() const {return text_;}

        enum class Key {Enter, Arrow};
        void activate(Key);

        private:
        void executeCommands_();

        friend class vix::command::Open;
        model::Selection &selection_;
        bool isCommand() const {return !text_.empty() && text_[0] == ':';}
        std::string getCommand() const {return text_.empty() ? "" : text_.substr(1);}
        std::string text_;

        typedef std::deque<ICommand::Ptr> Commands;
        Commands pendingCommands_;
        Commands executedCommands_;
    };
}

#endif
