#ifndef gubg_statemachine_Statemachine_hpp
#define gubg_statemachine_Statemachine_hpp

#include <memory>

//This module provides state machine functionality:
// * Dispatching and processing of difference _types_ of events
// * Hierarchical setup
//
//In general, an event arrives at the StateMachine via dispatchEvent(), which dispatches the event to its state for processing
//You give the StateMachine template a Policy class that handles storage and changing states
//
//This state, which derives from EventProcessor<Policy, Events> can either handle the event itself (e.g., if it is a leaf state)
//or dispatch the event to a more low-level state. In the latter case, the state derives also from StateMachine<Policy2, Events>

namespace gubg
{
    namespace statemachine
    {
        //Interfaces:
        // * EventDispatcher<Events>
        template <typename ...Events>
            struct EventDispatcher;
        template <typename Event>
            struct EventDispatcher<Event>
            {
                virtual bool dispatchEvent(Event) = 0;
            };
        template <typename Event, typename ...OtherEvents>
            struct EventDispatcher<Event, OtherEvents...>: EventDispatcher<OtherEvents...>
            {
                EventDispatcher<OtherEvents...>::dispatchEvent;
                virtual bool dispatchEvent(Event) = 0;
            };
        // * EventProcessor<Parent, Events>
        template <typename Parent, typename ...Events>
            struct EventProcessor;
        template <typename Parent, typename Event>
            struct EventProcessor<Parent, Event>
            {
                virtual bool processEvent(Event, Parent &) = 0;
            };
        template <typename Parent, typename Event, typename ...OtherEvents>
            struct EventProcessor<Parent, Event, OtherEvents...>: EventProcessor<Parent, OtherEvents...>
            {
                EventProcessor<Parent, OtherEvents...>::processEvent;
                virtual bool processEvent(Event, Parent &) = 0;
            };

        //StateMachine
        template <typename StatePolicy, typename ...Events>
            struct StateMachine;
        template <typename StatePolicy, typename Event>
            struct StateMachine<StatePolicy, Event>: StatePolicy, EventDispatcher<Event>
            {
                typedef EventProcessor<StatePolicy, Event> State;

                virtual bool dispatchEvent(Event event)
                {
                    return StatePolicy::state->processEvent(event, *this);
                }
            };
        template <typename StatePolicy, typename Event1, typename Event2>
            struct StateMachine<StatePolicy, Event1, Event2>: StatePolicy, EventDispatcher<Event1, Event2>
            {
                typedef EventProcessor<StatePolicy, Event1, Event2> State;

                virtual bool dispatchEvent(Event1 event)
                {
                    return StatePolicy::state->processEvent(event, *this);
                }
                virtual bool dispatchEvent(Event2 event)
                {
                    return StatePolicy::state->processEvent(event, *this);
                }
            };
    }
}

#endif
