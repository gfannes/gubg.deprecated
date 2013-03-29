#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <queue>
#include <cassert>

#define GUBG_MODULE "StateMachine"
#include "gubg/log/begin.hpp"
namespace gubg
{
    template <typename Outer, typename StateT, StateT StartState>
        class StateMachine_ftop
        {
            public:
                StateMachine_ftop(Outer &outer): state_(outer) { }

                class State
                {
                    public:
                        StateT operator()() const {return s_;}
                        void changeTo(StateT ns)
                        {
                            outer_.sm_exit(s_);
                            s_ = ns;
                            outer_.sm_enter(s_);
                        }
                    private:
                        friend class StateMachine_ftop;
                        State(Outer &outer): outer_(outer), s_(StartState) { }
                        State(const State &);
                        State &operator=(const State &);

                        Outer &outer_;
                        StateT s_;
                };

                template <typename Event>
                    void process(const Event &event)
                    {
                        SS(event);

                        if (doStart_())
                            state_.outer_.sm_enter(state_());

                        state_.outer_.sm_event(state_, event);
                    }

            private:
                State state_;
                OnlyOnce doStart_;
        };
}
#include "gubg/log/end.hpp"

#endif
