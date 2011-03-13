module gubg.StateMachine;

//General interface that handles an event
//True indicates that the event could be handled
interface IEventHandler(Event)
{
    //Returns true if the event is handled
    bool processEvent(Event event);
}

//Normal state machine base class with support for hooking into state changes via enterState()
class StateMachine(Event, State): IEventHandler!(Event)
{
    this (State state)
    {
        //You don't see when the first state is entered, just to make sure you don't accidently
        //access state_ before it is initialized
        state_ = state;
    }

    State state() {return state_;}

    //Implement this to define how events should affect the state
    abstract bool processEvent(Event event);

    protected:
    //Override if you need specific functionality when a state is entered
    void enterState(State newState){}
    //Call this helper method to actually change the state. It will give enterState()
    //the opportunity to handle the newly entered state
    final void changeState(State newState)
    {
        enterState(newState);
        state_ = newState;
    }

    private:
    State state_;
}

//Meta state machine; a state machine for which the states themselves are state machines (or at least an IEventHandler)
class MetaStateMachine(Event, Submachine = IEventHandler!(Event)): StateMachine!(Event, Submachine)
{
    this (Submachine startSubmachine)
    {
        super(startSubmachine);
    }
    final bool processEvent(Event event)
    {
        //First, we give the event to the submachine, which is the state
        if (submachine && submachine.processEvent(event))
            return true;

        //The submachine could not handle the event, lets try to handle it ourselves
        return processEventLocally(event);
    }
    //The state _is_ the submachine
    alias state submachine;

    protected:
    //Should return true if it could handle the event
    abstract bool processEventLocally(Event event);
}

version (UnitTest)
{
    import std.stdio;

    void main()
    {
        enum Event {Forward, Backward, Next};
        //Reacts on the Forward and Backward events
        class Stepper: StateMachine!(Event, int)
        {
            this (int step)
            {
                step_ = step;
                super(0);
            }
            bool processEvent(Event event)
            {
                switch (event)
                {
                    case Event.Forward:  changeState(state + step_); break;
                    case Event.Backward: changeState(state - step_); break;
                    default: return false; break;
                }
                return true;
            }
            void enterState(int newState)
            {
                writefln("Going from %s to %s", state, newState);
            }

            private:
            int step_;
        }

        //Test of the Stepper SM directly
        {
            writeln("Testing Stepper:");
            auto sm = new Stepper(2);
            writefln("Starting state: %s", sm.state);
            auto events = [Event.Forward, Event.Forward, Event.Backward];
            foreach (event; events)
            {
                sm.processEvent(event);
                writefln("Event: %s, state after event: %s", event, sm.state);
            }
        }

        //The meta SM that reacts on Event.Next by taking a new Stepper with a bigger step
        class MetaStepper: MetaStateMachine!(Event, Stepper)
        {
            this ()
            {
                step_ = 1;
                super(createStepper_);
            }
            protected:
            bool processEventLocally(Event event)
            {
                if (Event.Next != event)
                    return false;
                ++step_;
                changeState(createStepper_);
                return true;
            }
            private:
            Stepper createStepper_() {return new Stepper(step_);}
            int step_;
        }

        //Test the MetaStepper
        {
            writeln("\nTesting MetaStepper:");
            auto mm = new MetaStepper;
            writefln("Starting state: %s", mm.submachine.state);
            auto events = [Event.Forward, Event.Forward, Event.Next, Event.Forward, Event.Forward, Event.Next];
            foreach (event; events)
            {
                mm.processEvent(event);
                writefln("Event: %s, state: %s", event, mm.submachine.state);
            }
        }
    }
}
