#ifndef HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_StateMachine_hpp_ALREADY_INCLUDED

namespace gubg
{
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
}

#endif
