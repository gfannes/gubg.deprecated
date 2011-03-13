#ifndef gubg_statemachine_hpp
#define gubg_statemachine_hpp

#include <memory>

namespace gubg
{
    //General interface that handles an event
    //True indicates that the event could be handled
    template <typename Event>
    struct IEventHandler
    {
        //Returns true if the event is handled
        virtual bool processEvent(Event event) = 0;
    };

    //Normal state machine base class with support for hooking into state changes via enterState()
    template <typename Event, typename State>
    struct StateMachine: IEventHandler<Event>
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

    //Meta state machine; a state machine for which the states themselves are state machines (std::shared_ptr of something that has IEventHandler as its base)
    template <typename Event, typename Submachine = IEventHandler<Event> >
    struct MetaStateMachine: StateMachine<Event, std::shared_ptr<Submachine> >
    {
        typedef StateMachine<Event, std::shared_ptr<Submachine> > BaseT;

        MetaStateMachine(std::shared_ptr<Submachine> startSubmachine):
            BaseT(startSubmachine){}

        bool processEvent(Event event)
        {
            //First, we give the event to the submachine, which is the state
            if (submachine() && submachine()->processEvent(event))
                return true;

            //The submachine could not handle the event, lets try to handle it ourselves
            return processEventLocally(event);
        }
        //The state _is_ the submachine
        std::shared_ptr<Submachine> &submachine(){return BaseT::state();}

        protected:
        //Should return true if it could handle the event
        virtual bool processEventLocally(Event event) = 0;
    };
}

#endif
