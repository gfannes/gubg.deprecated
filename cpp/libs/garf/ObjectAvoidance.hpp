#ifndef HEADER_garf_ObjectAvoidance_hpp_ALREADY_INCLUDED
#define HEADER_garf_ObjectAvoidance_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"
#include "Arduino.h"

namespace garf
{
    template <typename Receiver>
    class ObjectAvoidance_crtp
    {
        public:
            enum class State {Forward, DetLeft, DetRight, Reverse, TurnRight, TurnLeft};

            ObjectAvoidance_crtp():
                sm_(*this),
                prev_(0){}

            void init()
            {
                prev_ = micros();
            }

            State debug_getState() const {return sm_.debug_getState();}

            void process()
            {
                unsigned long current = micros();
                sm_.process(current-prev_);
                prev_ = current;
            }

            typedef gubg::StateMachine_ftop<ObjectAvoidance_crtp, State, State::Forward> SM;
            SM sm_;
            void sm_enter(typename SM::State &s)
            {
                timer_ = 0;
                switch (s())
                {
                    case State::Forward:
                        receiver_().oa_left(100);
                        receiver_().oa_right(100);
                        break;
                    case State::DetLeft:
                        receiver_().oa_left(100);
                        receiver_().oa_right(-75);
                        break;
                    case State::DetRight:
                        receiver_().oa_left(-75);
                        receiver_().oa_right(100);
                        break;
                    case State::Reverse:
                        receiver_().oa_left(-75);
                        receiver_().oa_right(-75);
                        break;
                    case State::TurnRight:
                        receiver_().oa_left(100);
                        receiver_().oa_right(-100);
                        break;
                    case State::TurnLeft:
                        receiver_().oa_left(-100);
                        receiver_().oa_right(100);
                        break;
                }
            }
            void sm_exit(State s){}
            void sm_event(typename SM::State &s, const unsigned long &delta)
            {
                timer_ += delta;
                switch (s())
                {
                    case State::Forward:
                        if (detectedLeft_())
                            s.changeTo(State::DetLeft);
                        else if (detectedRight_())
                            s.changeTo(State::DetRight);
                        break;
                    case State::DetLeft:
                        if(timer_ > 1000000)
                            s.changeTo(State::Reverse);
                        else if (not detectedLeft_())
                            s.changeTo(State::Forward);
                        else if (detectedRight_())
                            s.changeTo(State::Reverse);
                        break;
                    case State::DetRight:
                        if(timer_ > 1000000)
                            s.changeTo(State::Reverse);
                        else if (not detectedRight_())
                            s.changeTo(State::Forward);
                        else if (detectedLeft_())
                            s.changeTo(State::Reverse);
                        break;
                    case State::Reverse:
                        if(timer_ > 1000000 || !digitalRead(42))
                        {        
                            if ((timer_/16) % 2)
                                s.changeTo(State::TurnRight);
                            else
                                s.changeTo(State::TurnLeft);
                        }
                        break;
                    case State::TurnRight:
                    case State::TurnLeft:
                        if (timer_ > 1000000)
                            s.changeTo(State::Forward);
                        break;
                }
            }

        private:
            Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            bool detectedLeft_(){return receiver_().oa_detectedLeft();}
            bool detectedRight_(){return receiver_().oa_detectedRight();}

            unsigned long prev_;
            unsigned long timer_;
    };
}

#endif
