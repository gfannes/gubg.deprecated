#include "gubg/StateMachine.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"

namespace 
{
    class SM
    {
        public:
            enum class State {Init, Start, Stop};

            SM():sm_(*this){}

            void process()
            {
                sm_.process();
                sm_.changeState(State::Stop);
            }

            void sm_enter(State s)
            {
                L("Enter " << (int)s);
            }
            void sm_exit(State s)
            {
                L("Exit " << (int)s);
            }
        private:
            gubg::StateMachine_ftop<SM, State::Start> sm_;
    };
}

int main()
{
    SM sm;

    sm.process();
    sm.process();
    sm.process();
    return 0;
}
