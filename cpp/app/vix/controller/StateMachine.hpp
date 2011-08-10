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
    struct Event
    {
        enum Type {CharT, SpecialT};
        enum Special {Backspace, Enter, Escape};
        union Value
        {
            char ch;
            Special special;
        };

        Event(Special);
        Event(char);

        Type type;
        Value value;
    };

    class StateMachine: public gubg::StateMachine<Event, std::string>
    {
        private:
            typedef gubg::StateMachine<Event, std::string> Base;

        public:
            StateMachine();

            virtual bool processEvent(Event);
            virtual void enterState(std::string);

            typedef boost::signals2::signal<void (std::string *)> Signal;
            typedef Signal::slot_type Slot;
            void connect(const Slot &);

        protected:
            virtual bool processChar(char);
            virtual bool processControl(Event::Special);

            Signal signal_;
    };
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
