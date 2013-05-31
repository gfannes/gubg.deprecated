#ifndef HEADER_garf_Sonars_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sonars_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"
#include "Arduino.h"

namespace garf
{
    template <int TriggerL, int TriggerR, int Echo, unsigned long Max>
        class Sonars
        {
            public:
                enum class State {Init, Idle, SendPulse, WaitForEchoReady, WaitForEcho, EchoReceived, EchoTooLate, Error, ReleaseMutex};
                enum class Which {Left, Right};

                Sonars():
                    sm_(*this),
                    prev_(micros()){}
                void init()
                {
                    prev_ = micros();
                }

                State debug_getState() const {return sm_.debug_getState();}

                bool process(Which &w, unsigned long &distance)
                {
                    bool res = false;
                    if (sm_.checkState(State::EchoReceived))
                    {
                        //Raw conversion from us to cm
                        distance = timer_/80;
                        w = which_;
                        res = true;
                    }
                    unsigned long current = micros();
                    sm_.process(current-prev_);
                    prev_ = current;
                    return res;
                }

                typedef gubg::StateMachine_ftop<Sonars, State, State::Init> SM;
                SM sm_;
                void sm_enter(State s)
                {
                    switch (s)
                    {
                        case State::Init:
                            pinMode(TriggerL, OUTPUT);
                            pinMode(TriggerR, OUTPUT);
                            pinMode(Echo, INPUT);
                            which_ = Which::Right;
                            break;
                        case State::Idle:
                            switchSide_();
                            digitalWrite(triggerPin_(), LOW);
                            timer_ = 0;
                            break;
                        case State::SendPulse:
                            digitalWrite(triggerPin_(), HIGH);
                            delayMicroseconds(20);
                            digitalWrite(triggerPin_(), LOW);
                            timer_ = 0;
                            break;
                        case State::ReleaseMutex:
                            break;
                    }
                }
                void sm_exit(State s){}
                void sm_event(typename SM::State &s, const unsigned long &delta)
                {
                    timer_ += delta;
                    switch (s())
                    {
                        case State::Init:
                            s.changeTo(State::Idle);
                            break;
                        case State::Idle:
                            if (timer_ >= 1000 && (digitalRead(Echo) == LOW))
                                s.changeTo(State::SendPulse);
                            break;
                        case State::SendPulse:
                            s.changeTo(State::WaitForEchoReady);
                            break;
                        case State::WaitForEchoReady:
                            if (timer_ > Max*33)
                                s.changeTo(State::Error);
                            else if (digitalRead(Echo) == HIGH)
                                s.changeTo(State::WaitForEcho);
                            break;
                        case State::WaitForEcho:
                            if (timer_ > Max*33)
                                s.changeTo(State::EchoTooLate);
                            else if (digitalRead(Echo) == LOW)
                                s.changeTo(State::EchoReceived);
                            break;
                        case State::EchoReceived:
                        case State::EchoTooLate:
                        case State::Error:
                            s.changeTo(State::ReleaseMutex);
                            break;
                        case State::ReleaseMutex:
                            s.changeTo(State::Idle);
                            break;
                    }
                }

            private:
                void switchSide_()
                {
                    switch (which_)
                    {
                        case Which::Left: which_ = Which::Right; break;
                        case Which::Right: which_ = Which::Left; break;
                    }
                }
                int triggerPin_() const
                {
                    switch (which_)
                    {
                        case Which::Left: return TriggerL; break;
                        case Which::Right: return TriggerR; break;
                    }
                    return 0;
                }
                unsigned long prev_;
                unsigned long timer_;
                Which which_;
        };
}

#endif
