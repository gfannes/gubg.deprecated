#include "controller/StateMachine.hpp"
using namespace std;

namespace vix
{
    //Event constructors
    Event::Event(Special special):
        type(SpecialT)
    {
        value.special = special;
    }
    Event::Event(char ch):
        type(CharT)
    {
        value.ch = ch;
    }

    //StateMachine public methods
    StateMachine::StateMachine():
        Base(""){}
    bool StateMachine::processEvent(Event event)
    {
        switch (event.type)
        {
            case Event::CharT:
                return processChar(event.value.ch);
                break;
            case Event::SpecialT:
                return processControl(event.value.special);
                break;
        }
        return false;
    }
    void StateMachine::enterState(string str)
    {
        signal_(&str);
    }
    void StateMachine::connect(const Slot &subscriber)
    {
        signal_.connect(subscriber);
    }
    //StateMachine private methods
    bool StateMachine::processChar(char ch)
    {
        changeState(state()+ch);
        return true;
    }
    bool StateMachine::processControl(Event::Special special)
    {
        switch (special)
        {
            case Event::Backspace:
                {
                    auto s = state();
                    if (!s.empty())
                        changeState(s.substr(0, s.size()-1));
                }
                break;
            case Event::Enter:
                break;
            case Event::Escape:
                if (state().empty())
                    return false;
                changeState("");
                break;
            default:
                return false;
                break;
        }
        return true;
    }

    //FilterStateMachine methods
    bool FilterStateMachine::processChar(char ch)
    {
        switch (ch)
        {
            case ':':
            case '/':
                return false;
        }
        return StateMachine::processChar(ch);
    }
    bool FilterStateMachine::processControl(Event::Special special)
    {
        return StateMachine::processControl(special);
    }

    //ContentStateMachine methods
    bool ContentStateMachine::processChar(char ch)
    {
        return StateMachine::processChar(ch);
    }
    bool ContentStateMachine::processControl(Event::Special special)
    {
        return StateMachine::processControl(special);
    }

    //CommandStateMachine methods
    bool CommandStateMachine::processChar(char ch)
    {
        return StateMachine::processChar(ch);
    }
    bool CommandStateMachine::processControl(Event::Special special)
    {
        return StateMachine::processControl(special);
    }
}
