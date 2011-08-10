#include "statemachine/StateMachine.hpp"
#include <iostream>
#include <array>
using namespace gubg;
using namespace std;

enum class Event {Forward, Backward, Next};
ostream &operator<<(ostream &os, const Event &event)
{
    switch (event)
    {
        case Event::Forward: os << "Forward"; break;
        case Event::Backward: os << "Backward"; break;
        case Event::Next: os << "Next"; break;
    }
    return os;
}

int main()
{
    //Reacts on the Forward and Backward events
    struct Stepper: StateMachine<Event, int>
    {
        Stepper(int step):
            StateMachine(0),
            step_(step){}

        virtual bool processEvent(Event event)
        {
            switch (event)
            {
                case Event::Forward:  changeState(state() + step_); break;
                case Event::Backward: changeState(state() - step_); break;
                default: return false; break;
            }
            return true;
        }
        virtual void enterState(int newState)
        {
            cout << "Going from " << state() << " to " << newState << endl;
        }

        private:
        int step_;
    };

    //Test of the Stepper SM directly
    {
        cout << "Testing Stepper:" << endl;
        Stepper sm(2);
        cout << "Starting state: " << sm.state() << endl;
        std::array<Event, 3> events = {Event::Forward, Event::Forward, Event::Backward};
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            Event &event = events[i];
            sm.processEvent(event);
            cout << "Event: " << event << " state after event: " << sm.state() << endl;
        }
    }

    //The meta SM that reacts on Event::Next by taking a new Stepper with a bigger step
    struct MetaStepper: MetaStateMachine<Event, Stepper>
    {
        MetaStepper():
            MetaStateMachine(shared_ptr<Stepper>()),
            step_(1)
        {
            changeState(createStepper_());
        }

        protected:
        bool processEventLocally(Event event)
        {
            if (Event::Next != event)
                return false;
            ++step_;
            changeState(createStepper_());
            return true;
        }

        private:
        shared_ptr<Stepper> createStepper_() {return shared_ptr<Stepper>(new Stepper(step_));}
        int step_;
    };

    //Test the MetaStepper
    {
        cout << "\nTesting MetaStepper:" << endl;
        MetaStepper mm;
        cout << "Starting state: " << mm.submachine()->state() << endl;
        array<Event, 6> events = {Event::Forward, Event::Forward, Event::Next, Event::Forward, Event::Forward, Event::Next};
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            Event &event = events[i];
            mm.processEvent(event);
            cout << "Event: " << event << ", state: " << mm.submachine()->state() << endl;
        }
    }
    return 0;
}
