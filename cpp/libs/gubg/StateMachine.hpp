#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

//#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/OnlyOnce.hpp"
#include <queue>
#include <cassert>
#include "gubg/l.hpp"

namespace gubg
{
    template <typename Outer, typename StateT, StateT StartState>
        class StateMachine_ftop
        {
            public:
                StateMachine_ftop(Outer &outer):outer_(outer) { }

                class State
                {
                    public:
                        State(StateMachine_ftop &sm, StateT &s):sm_(sm), s_(s){}
                        StateT operator()() const {return s_;}
                        void changeTo(StateT ns)
                        {
                            sm_.outer_.sm_exit(s_);
                            s_ = ns;
                            sm_.outer_.sm_enter(s_);
                        }
                    private:
                        StateMachine_ftop &sm_;
                        StateT &s_;
                };
                template <typename Event>
                    void process(const Event &event)
                    {
                        LOG_S(process, STREAM(event));

                        if (doStart_())
                        {
                            state_ = StartState;
                            outer_.sm_enter(state_);
                        }

                        State state(*this, state_);
                        outer_.sm_event(state, event);
                    }
            private:
                Outer &outer_;
                StateT state_;
                OnlyOnce doStart_;
        };
}

#endif
