#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include <queue>
#include <cassert>
#include "gubg/l.hpp"

namespace gubg
{
    template <typename Outer, typename StateT>
        class StateMachine_ftop
        {
            public:
                typedef StateT State;

                StateMachine_ftop(Outer &outer):outer_(outer), state_(State::OK){}
                template <typename Event>
                    void process(const Event &event)
                    {
                        LOG_S(process, STREAM(event));
                        auto newState = outer_.sm_event(state_, event);
                        while (newState != State::OK)
                        {
                            outer_.sm_exit(state_, event);
                            state_ = newState;
                            newState = outer_.sm_enter(state_, event);
                        }
                    }
            private:
                Outer &outer_;
                State state_;
        };
#if 0
    template <typename Outer, typename StateT, StateT StartState>
        class SM
        {
            public:
                typedef StateT State;

                //In-house events
                struct Start {};
                template <State WantedState>
                    struct ChangeState { };

                SM(Outer &outer):outer_(outer), state_fp(0){}

                template <typename Event>
                    void process(const Event &event)
                    {
                        startIfUnstarted_();
                        L("Processing event " << &event << ", state " << state_fp << " " << (int)(this->*state_fp)());
                        eventQueue_.push(&event);
                        action_fp = &(SM::action_ft<Event>);
                        (this->*action_fp)();
                    }
                template <State S>
                    void process(const ChangeState<S> &)
                    {
                        startIfUnstarted_();
                        L("Trying to change state to " << (int)S);
                    }

            private:
                Outer &outer_;

                typedef State (SM::*StateFP)() const;
                StateFP state_fp;
                template <State S>
                    State state_ft() const
                    {
                        return S;
                    }

                typedef void (SM::*ActionFP)();
                ActionFP action_fp;
                template <typename Event>
                    void action_ft()
                    {
                        outer_.sm_action(event_<Event>());
                    }

                typedef void (SM::*EnterFP)();
                EnterFP enter_fp;
                template <State S>
                    struct StateInfo
                    {
                        SM &o_;
                        StateInfo(SM &o):o_(o){}
                        template <State NewState>
                            void changeTo()
                            {
                            }
                    };
                template <State S, typename Event>
                    void enter_ft()
                    {
                        StateInfo<S> si(*this);
                        outer_.sm_enter(event_<Event>(), si);
                    }

                //When you push an event, make sure it does not get out of scope, we just store its address
                typedef std::queue<const void *> EventQueue;
                EventQueue eventQueue_;
                template <typename Event>
                    const Event &event_()
                    {
                        const Event &event = *reinterpret_cast<const Event*>(eventQueue_.front());
                        eventQueue_.pop();
                        return event;
                    }

                void startIfUnstarted_()
                {
                        if (!state_fp)
                        {
                            L("Starting state machine");
                            state_fp = &(SM::state_ft<StartState>);
                            Start startEvent;
                            eventQueue_.push(&startEvent);
                            enter_fp = &(SM::enter_ft<StartState, Start>);
                            (this->*enter_fp)();
                        }
                }
        };

    template <typename Outer, typename Outer::State StartState>
        class StateMachine_ftop
        {
            public:
                typedef typename Outer::State State;

                StateMachine_ftop(Outer &outer):
                    outer_(outer), state_(State::Init){}

                State state() const {return state_;}

                void process()
                {
                    initialize_();
                }

                void changeState(const State newState)
                {
                    if (newState == state_)
                        //No state change
                        return;
                    if (state_ == State::Init)
                        //You have to process first to get out of the Init state
                        return;
                    if (newState == State::Init)
                        //You cannot enter the Init state again
                        return;

                    outer_.sm_exit(state_);
                    state_ = newState;
                    outer_.sm_enter(state_);
                }

            private:
                void initialize_()
                {
                    if (state_ != State::Init)
                        return;
                    state_ = StartState;
                    outer_.sm_enter(state_);
                }

                Outer &outer_;
                State state_;
        };
#endif
}

#endif
