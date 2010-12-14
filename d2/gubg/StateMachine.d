module gubg.StateMachine;

class StateMachine(Event, State)
{
    this (State state)
    {
        //You don't see when the first state is entered, just to make sure you don't accidently
        //access state_ before it is initialized
        state_ = state;
    }

    State state() {return state_;}

    //Implement this to define how events should affect the state
    abstract void processEvent(Event event);

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

version (UnitTest)
{
    import std.stdio;

    void main()
    {
        enum Event {Forward, Backward};
        class Counter: StateMachine!(Event, int)
        {
            this () { super(0); }
            void processEvent(Event event)
            {
                switch (event)
                {
                    case Event.Forward:  changeState(state+1); break;
                    case Event.Backward: changeState(state-1); break;
                }
            }
            void enterState(int newState)
            {
                writefln("Going from %s to %s", state, newState);
            }
        }
        auto sm = new Counter;
        writefln("Starting state: %s", sm.state);
        auto events = [Event.Forward, Event.Forward, Event.Backward];
        foreach (event; events)
        {
            sm.processEvent(event);
            writefln("Event: %s, state after event: %s", event, sm.state);
        }
    }
}
