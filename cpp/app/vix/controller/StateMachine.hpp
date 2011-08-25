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

    //MetaMachine
    struct MetaState
    {
        typedef gubg::statemachine::EventProcessor<MetaState, char, Special> State;

        void changeState(Control control);
        State *state;
        State *filter;
        State *content;
        State *command;
    };
    typedef gubg::statemachine::StateMachine<MetaState, char, Special> MetaMachine;

    //EditableString
    struct EditableString: MetaState::State, gubg::statemachine::EventDispatcher<char, Special>
    {
        virtual bool processEvent(char ch, MetaState &);
        virtual bool processEvent(Special event, MetaState &ms);
        virtual bool dispatchEvent(char ch);
        virtual bool dispatchEvent(Special event);
        std::string state;

        typedef boost::signals2::signal<void (const std::string *)> Signal;
        typedef Signal::slot_type Slot;
        void connect(const Slot &subscriber);
        void signalStateUpdate_() const;
        Signal signal_;
    };

    //The different filters
    class FilterStateMachine: public EditableString
    {
    };
    class ContentStateMachine: public EditableString
    {
    };
    class CommandStateMachine: public EditableString
    {
    };
}

#endif
