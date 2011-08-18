#ifndef vix_controller_StateMachine_hpp
#define vix_controller_StateMachine_hpp

#include "statemachine/StateMachine.hpp"
#include "boost/signals2.hpp"
#include <string>

namespace vix
{
    enum class Control
    {
        Filter = 0,
        Content = 1,
        Command = 2,
        NrValues = 3
    };

    //Events:
    // * char
    // * Special
    enum Special {Backspace, Enter, Escape};

    struct EditableStringPolicy
    {
        template <typename SM>
        bool dispatchEvent(char ch, SM &sm)
        {
            auto str = sm.state();
            str.append(ch);
            sm.changeState(str);
            return true;
        }
        template <typename SM>
        bool dispatchEvent(Special event, SM &sm)
        {
            if (Backspace != event)
                return false;
            auto str = sm.state();
            sm.changeState(str.substr(0, (str.empty() ? 0 : str.size()-1)));
            return true;
        }
    };
    struct EditableString: gubg::statemachine::StateMachine<std::string, EditableStringPolicy>
    {
        typedef boost::shared_ptr<EditableString> Ptr;
    };

    struct MetaPolicy: gubg::statemachine::DispatchToState
    {
    };
    typedef gubg::statemachine::StateMachine<EditableString::Ptr, MetaPolicy> MetaMachine;

    class FilterStateMachine: public StateMachine
    {
        protected:
            virtual bool processChar(char);
            virtual bool processControl(Event::Special);
    };
    class ContentStateMachine: public StateMachine
    {
        protected:
            virtual bool processChar(char);
            virtual bool processControl(Event::Special);
    };
    class CommandStateMachine: public StateMachine
    {
        protected:
            virtual bool processChar(char);
            virtual bool processControl(Event::Special);
    };
}

#endif
