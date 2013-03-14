#include "gubg/StateMachine.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
#include <string>
using namespace std;

namespace 
{
    class A
    {
        public:
            A():sm(*this){}
            void process()
            {
                sm.process(0);
                sm.process("abc");
            }
        private:
            enum class State {Idle, Flip};
            typedef gubg::StateMachine_ftop<A, State, State::Idle> SM;
            friend class gubg::StateMachine_ftop<A, State, State::Idle>;
            SM sm;
            void sm_event(SM::State &s, int e)
            {
                L("Event " << e << " occured in state " << (int)s());
                switch (s())
                {
                    case State::Idle: s.changeTo(State::Flip); break;
                    case State::Flip:
                                      if (e < 4)
                                          sm.process(e+1);
                                      break;
                }
            }
            void sm_event(SM::State &s, const string &str)
            {
                L("Event " << str << " occured in state " << (int)s());
                switch (s())
                {
                    case State::Idle: s.changeTo(State::Flip); break;
                    case State::Flip:
                                      if (!str.empty())
                                          sm.process(str.substr(1));
                                      break;
                }
            }
            void sm_exit(State s)
            {
                L("Leaving state " << (int)s);
            }
            void sm_enter(State s)
            {
                L("Entering state " << (int)s);
                switch (s)
                {
                    case State::Idle: sm.process(0); break;
                }
            }
    };
}
int main()
{
    A a;
    a.process();
    a.process();
    a.process();
    return 0;
}
