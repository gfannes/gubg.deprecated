#ifndef HEADER_gubg_statemachine_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_statemachine_StateMachine_hpp_ALREADY_INCLUDED

#include <memory>
#include <iostream>

namespace gubg
{
    namespace statemachine
    {
        using namespace std;

        //Different machine types, use a typedef to indicate what machine you are implementing
        enum Leaf{};
        enum Hyper{};

        //A state holder based on a smart pointer
        template <typename State>
            struct SmartStateT
            {
                typedef State Type;
                template <typename Event>
                    bool process(Event event)
                    {
                        //We forward the event to the state, and pass ourselves as StateMgr because we hold the state and know how to change it
                        return state->process(event, *this);
                    }
                void changeState(State *newState)
                {
                    state.reset(newState);
                }
                auto_ptr<State> state;
            };

        //The basic statemaching template
        template <typename StatePolicy>
            struct StateMachineT: StatePolicy
        {
            template <typename... Args>
                StateMachineT(Args... args):
                    StatePolicy(args...){}

            //An event enters the system, without a specification how the state can be changed.
            //We use the StatePolicy to change a state
            //Typically, when a state handles an event, it will need information from above to know how to change the state
            //A state knows the next state based on some event, but it doesn't know how states are stored or changed; this is StateMgr
            template <typename Event>
                bool process(Event event)
                {
                    return process(event, *static_cast<StatePolicy*>(this), (typename StatePolicy::MachineType*)(0));
                }
            //We import those process methods provided by the StatePolicy, which should stop the template selection machinery
            StatePolicy::process;

            //A Hyper machine goes via the StateMgr to process the event
            template <typename Event, typename StateMgr>
                bool process(Event event, StateMgr &sm, Hyper *p)
                {
                    return sm.process(event);
                }
            //A Leaf machine handles the event directly
            template <typename Event, typename StateMgr>
                bool process(Event event, StateMgr &sm, Leaf *p)
                {
                    return process(event, sm);
                }
            //This is a catch-all from the StateMgr, we will discard the StateMgr and try again to see if we find a match
            template <typename Event, typename StateMgr>
                bool process(Event event, StateMgr &sm)
                {
                    return process(event);
                }
        };
    }
}

#endif
