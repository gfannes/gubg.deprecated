#ifndef gubg_statemachine_hpp
#define gubg_statemachine_hpp

namespace gubg
{
    template <typename Event>
    class IEventHandler
    {
        //Returns true if the event is handled
        virtual bool processEvent(Event event) = 0;
    };

    //Normal state machine base class with support for hooking into state changes
    template <typename Event, typename State>
    class StateMachine: public IEventHandler<Event>
    {
        StateMachine(State state):
            //You don't see when the first state is entered, just to make sure you don't accidently
            //access state_ before it is initialized
            state_(state){}

        State &state() {return state_;}

        //Implement this to define how events should affect the state
        virtual bool processEvent(Event event) = 0;

        protected:
        //Override if you need specific functionality when a state is entered
        virtual void enterState(State newState){}
        //Call this helper method to actually change the state. It will give enterState()
        //the opportunity to handle the newly entered state
        void changeState(State newState)
        {
            enterState(newState);
            state_ = newState;
        }

        private:
        State state_;
    };

    //Meta state machine; a state machine for which the states themselves are state machines (or at least an IEventHandler)
    template <typename Event, typename Submachine = IEventHandler<Event>* >
    class MetaStateMachine: public StateMachine<Event, Submachine>
    {
        MetaStateMachine(Submachine startSubmachine):
            StateMachine<Event, Submachine>(startSubmachine){}

        bool processEvent(Event event)
        {
            //First, we give the event to the submachine, which is the state
            if (state() && state()->processEvent(event))
                return true;

            //The submachine could not handle the event, lets try to handle it ourselves
            return processEventLocally(event);
        }

        protected:
        //Should return true if it could handle the event
        virtual bool processEventLocally(Event event) = 0;
    };
}

#endif
