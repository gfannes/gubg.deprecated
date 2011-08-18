#include "statemachine/StateMachine.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
#include <iostream>
#include <array>
using namespace gubg;
using namespace std;

enum class Event {Forward, Backward};
ostream &operator<<(ostream &os, const Event &event)
{
    switch (event)
    {
        case Event::Forward: os << "Forward"; break;
        case Event::Backward: os << "Backward"; break;
    }
    return os;
}

struct MetaState
{
    typedef gubg::statemachine::EventProcessor<MetaState, Event, int> State;
    void changeState(int);
    State *state;
};
typedef gubg::statemachine::StateMachine<MetaState, Event, int> MetaMachine;

struct Stepper: gubg::statemachine::EventDispatcher<Event, int>, MetaMachine::State 
{
    Stepper(int s):
        state(0),
        step(s){}
    virtual bool processEvent(Event event, MetaState &meta)
    {
        return dispatchEvent(event);
    }
    virtual bool processEvent(int s, MetaState &meta)
    {
        LOG_SM(Stepper::processEvent, "s: " << s);
        meta.changeState(s);
        return true;
    }
    virtual bool dispatchEvent(Event event)
    {
        switch (event)
        {
            case Event::Forward: state += step; break;
            case Event::Backward: state -= step; break;
            default: return false; break;
        }
        return true;
    }
    virtual bool dispatchEvent(int){return false;}

    int state;
    int step;
};

void MetaState::changeState(int s)
{
    state = new Stepper(s);
}

namespace
{
    int bottomState(MetaMachine &mm)
    {
        return dynamic_cast<Stepper*>(mm.state)->state;
    }
}
int main()
{
    LOG_S(main);
    //Test of the Stepper SM directly
    {
        LOG_SM(Stepper, "Testing Stepper");
        Stepper sm(2);
        LOG_M("Starting state: " << sm.state);
        std::array<Event, 3> events = {Event::Forward, Event::Forward, Event::Backward};
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            Event &event = events[i];
            sm.dispatchEvent(event);
            LOG_M("Event: " << event << " state after event: " << sm.state);
        }
    }

    //Test the MetaStepper
    {
        LOG_SM(Stepper, "Testing MetaMachine");
        MetaMachine mm;
        mm.changeState(3);
        LOG_M("Starting state: " << bottomState(mm));
        array<Event, 8> events = {Event::Forward, Event::Forward, Event::Forward, Event::Forward, Event::Forward, Event::Backward};
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            Event &event = events[i];
            mm.dispatchEvent(event);
            if (1 == i%3)
            mm.dispatchEvent(i);
            LOG_M("Event: " << event << ", state: " << bottomState(mm));
        }
    }
    return 0;
}
