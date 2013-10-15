#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

//sm_enter() might be called for the init state when it is not really expected: upon checkState() or processing the first event; make sure its enter action is not too harmful

#include "gubg/OnlyOnce.hpp"
#include "gubg/Macro.hpp"

#define GUBG_MODULE "StateMachine"
#include "gubg/log/begin.hpp"
namespace gubg
{
#define DATA_EVENT_1(tn)                 struct tn {}
#define DATA_EVENT_2(_1, _2)             error 
#define DATA_EVENT_3(tn, t1, n1)         struct tn {t1 n1; tn(t1 ln1):n1(ln1){}}
#define DATA_EVENT_4(_1, _2, _3, _4)     error
#define DATA_EVENT_5(tn, t1, n1, t2, n2) struct tn {t1 n1; t2 n2; tn(t1 ln1, t2 ln2):n1(ln1),n2(ln2){}}
#define DATA_EVENT_MACRO_CHOOSER(...) GUBG_GET_6TH_ARG(__VA_ARGS__, DATA_EVENT_5,DATA_EVENT_4,DATA_EVENT_3,DATA_EVENT_2,DATA_EVENT_1)
#define DATA_EVENT(...) DATA_EVENT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

    template <typename Outer, typename StateT, StateT StartState>
        class StateMachine_ftop
        {
            public:
                StateMachine_ftop(Outer &outer): state_(outer) { }

                bool checkState(const StateT s) const
                {
                    if (doStart_.check())
                    {
                        //The state machine is not yet started, we are in no state yet
                        return false;
                    }
                    return s == state_.s_;
                }
                //Make sure the state machine is processed at least once before calling this
                StateT debug_getState() const
                {
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
                            //This is the first time we process an event, we still have to enter the initial state
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
