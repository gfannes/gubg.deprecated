#ifndef HEADER_garf_Sonar_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sonar_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"
#include "Arduino.h"

namespace garf
{
    template <int Trigger, int Echo, unsigned long Max>
        class Sonar
        {
            public:
                enum class State {Init, Idle, SendPulse, WaitForEchoReady, WaitForEcho, EchoReceived, EchoTooLate, Error, ReleaseMutex};

                //mutex == true => mutex is locked
                Sonar(bool &mutex):
                    sm_(*this),
                    prev_(micros()),
                    mutex_(mutex){}
                void init()
                {
                    prev_ = micros();
                }

                State debug_getState() const {return sm_.debug_getState();}

                bool process(unsigned long &distance)
                {
                    bool res = false;
                    if (sm_.checkState(State::EchoReceived))
                    {
                        //Raw conversion from us to cm
                        distance = timer_/80;
                        res = true;
                    }
                    unsigned long current = micros();
                    sm_.process(current-prev_);
                    prev_ = current;
                    return res;
                }

                typedef gubg::StateMachine_ftop<Sonar, State, State::Init> SM;
                SM sm_;
                void sm_enter(State s)
                {
                    switch (s)
                    {
                        case State::Init:
                            pinMode(Trigger, OUTPUT);
                            pinMode(Echo, INPUT);
                            break;
                        case State::Idle:
                            digitalWrite(Trigger, LOW);
                            timer_ = 0;
                            break;
                        case State::SendPulse:
                            mutex_ = true;
                            digitalWrite(Trigger, HIGH);
                            delayMicroseconds(20);
                            digitalWrite(Trigger, LOW);
                            timer_ = 0;
                            break;
                        case State::ReleaseMutex:
                            mutex_ = false;
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
                            if (!mutex_ && timer_ >= 1000 && (digitalRead(Echo) == LOW))
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
                unsigned long prev_;
                unsigned long timer_;
                bool &mutex_;
        };
}

#endif
