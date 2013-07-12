#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

//sm_enter() might be called for the init state when it is not really expected: upon checkState() or processing the first event; make sure its enter action is not too harmful

#include "gubg/OnlyOnce.hpp"

#define GUBG_MODULE_ "StateMachine"
#include "gubg/log/begin.hpp"
namespace gubg
{
#define DATA_EVENT(name) struct name {}

    template <typename Outer, typename StateT, StateT StartState>
        class StateMachine_ftop
        {
            public:
                StateMachine_ftop(Outer &outer): state_(outer) { }

                bool checkState(StateT s) const
                {
                    if (doStart_())
                        state_.outer_.sm_enter(state_);
                    return s == state_.s_;
                }
                StateT debug_getState() const
                {
                    if (doStart_())
                        state_.outer_.sm_enter(state_);
                    return state_.s_;
                }

                class State
                {
                    public:
                        StateT operator()() const {return s_;}
                        void changeTo(StateT ns)
                        {
                            S();
                            L("Leaving state " << (int)s_);
                            outer_.sm_exit(s_);
                            s_ = ns;
                            L("Entering state " << (int)s_);
                            outer_.sm_enter(*this);
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
                        if (doStart_())
                            state_.outer_.sm_enter(state_);

                        state_.outer_.sm_event(state_, event);
                    }

            private:
                State state_;
                OnlyOnce doStart_;
        };
}
#include "gubg/log/end.hpp"

#endif
