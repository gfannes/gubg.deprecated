#include "statemachine/StateMachine.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
#include <iostream>
#include <array>
using namespace gubg;
using namespace gubg::statemachine;
using namespace std;
#define L(m) cout<<m<<endl

enum Direction {Forward, Backward};
ostream &operator<<(ostream &os, Direction direction)
{
    switch (direction)
    {
        case Forward: return os << "Forward";
        case Backward: return os << "Backward";
    }
    return os;
}

struct Stepper
{
    typedef Leaf MachineType;
    Stepper(int s):
        state(0), step(s){L("\nNew stepper with step " << step);}
    //This will replace the meta-state using StateMgr
    template <typename StateMgr>
    bool process(int newStep, StateMgr &sm)
    {
        sm.changeState(new typename StateMgr::Type(newStep));
        return true;
    }
    //This is a basic operation
    bool process(Direction direction, Stepper &sm)
    {
        switch (direction)
        {
            case Forward: sm.changeState(state + step); break;
            case Backward: sm.changeState(state - step); break;
            default: return false; break;
        }
        return true;
    }

    void changeState(int newState)
    {
        int prev = state;
        state = newState;
        L("Changed state from " << prev << " to " << state << "");
    }
    const int step;
    int state;
};
typedef StateMachineT<Stepper> StepperSM;

struct MetaStepper: SmartStateT<StepperSM>
{
    typedef Hyper MachineType;
    MetaStepper(){changeState(1);}
    void changeState(int step){SmartStateT::changeState(new StepperSM(step));}
};

template <typename T>
int bottomState(T &t)
{
    L("bootom");
    return t.state->state;
}

int main()
{
    LOG_S(main);
    //Test of the Stepper SM directly
    {
        LOG_SM(Stepper, "Testing Stepper");
        Stepper sm(2);
        LOG_M("Starting state: " << sm.state);
        std::array<Direction, 3> events = {Direction::Forward, Direction::Forward, Direction::Backward};
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            Direction &event = events[i];
            sm.process(event, sm);
            LOG_M("Direction: " << event << " state after event: " << sm.state);
        }
    }

    //Test the MetaStepper
    {
        LOG_SM(Stepper, "Testing MetaMachine");
        typedef StateMachineT<MetaStepper> MetaMachine;
        MetaMachine mm;
        //mm.changeState(3);
        LOG_M("Starting state: " << bottomState(mm));
        array<Direction, 8> events = {Direction::Forward, Direction::Forward, Direction::Forward, Direction::Forward, Direction::Forward, Direction::Backward};
        //If the int hereunder is changed to unsigned int, there is an infinite loop in the statemachine code. No so nice...
        for (int i = 0; i < events.size(); ++i)
        {
            Direction &event = events[i];
            mm.process(event);
            if (1 == i%3)
                mm.process(i);
            LOG_M("Direction: " << event << ", state: " << bottomState(mm));
        }
    }
    return 0;
}
