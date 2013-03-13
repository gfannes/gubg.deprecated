#include "gubg/StateMachine.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"

#if 0
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
namespace 
{
    class A
    {
        public:
            A():sm(*this){}

            void process()
            {
                sm.process(0);
            }

        private:
            enum class State {Start, Flip};
            typedef gubg::SM<A, State, State::Start> SM;
            friend class gubg::SM<A, State, State::Start>;
            SM sm;
            template <typename S>
                void sm_enter(SM::Start, S &s)
                {
                    L("sm_enter for Start event");
                    sm.process(SM::ChangeState<State::Flip>());
                    s.template changeTo<State::Flip>();
                }
            void sm_action(int event)
            {
                L("What a nice int event " << event);
                if (event < 10)
                    sm.process(event+1);
            }
    };
}
#else
namespace 
{
    class A
    {
        public:
            A():sm(*this){}
            void process()
            {
                sm.process(0);
            }
        private:
            enum class State {OK, Idle, Flip};
            typedef gubg::StateMachine_ftop<A, State> SM;
            friend class gubg::StateMachine_ftop<A, State>;
            SM sm;
            State sm_event(State s, int e)
            {
                MSS_BEGIN(State);
                L("Event " << e << " occured in state " << (int)s);
                switch (s)
                {
                    case State::OK: MSS_QL(Idle); break;
                    case State::Flip: sm.process(e+1); break;
                }
                MSS_END();
            }
            void sm_exit(State s, int e){}
            State sm_enter(State s, int e)
            {
                MSS_BEGIN(State);
                L("Entering state " << (int)s << " via event " << e);
                switch (s)
                {
                    case State::Idle: MSS_QL(Flip); break;
                    case State::Flip: sm.process(e); break;
                }
                MSS_END();
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
#endif
