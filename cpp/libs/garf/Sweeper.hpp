#ifndef HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"
#include "Arduino.h"
#include "Servo/Servo.h"

namespace garf
{
    template <int Pin, int Min, int Max, int Delay>
        class Sweeper
        {
            public:
                Sweeper():
                    rest_(0),
                    sm_(*this){}

                void process(int elapse)
                {
                    rest_ += elapse;
                    while (rest_ > Delay)
                    {
                        rest_ -= Delay;
                        sm_.process(Tick());
                    }
                }

            private:
                enum class State {Init, Up, Down};
                struct Tick {};
                typedef Sweeper<Pin, Min, Max, Delay> Self;
                typedef gubg::StateMachine_ftop<Self, State, State::Init> SM;
                friend class gubg::StateMachine_ftop<Self, State, State::Init>;
                SM sm_;
                void sm_enter(typename SM::State &s) { }
                void sm_exit(State s)
                {
                    switch (s)
                    {
                        case State::Init:
                            //We initialize the servo as late as possible to make sure the Arduino
                            //lib is already initialized
                            servo_.attach(Pin);
                            pos_ = Min;
                            break;
                    }
                }
                void sm_event(typename SM::State &s, const Tick &)
                {
                    servo_.write(pos_);
                    switch (s())
                    {
                        case State::Init:
                            s.changeTo(State::Up);
                            break;
                        case State::Up:
                            ++pos_;
                            if (pos_ >= Max)
                                s.changeTo(State::Down);
                            break;
                        case State::Down:
                            --pos_;
                            if (pos_ <= Min)
                                s.changeTo(State::Up);
                            break;
                    }
                }

                int rest_;
                Servo servo_;
                int pos_;
        };
}

#endif
