#ifndef HEADER_garf_DualRail_hpp_ALREADY_INCLUDED
#define HEADER_garf_DualRail_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"
#include "garf/LED.hpp"

namespace garf
{
    enum class Role {Master, Slave};

    template <typename Receiver, int Bit0, int Bit1, typename Buffer>
        class DualRail
        {
            public:
                DualRail(): sm_(*this) {}

                bool isLineIdle() const
                {
                    //checkState also makes sure that the Idle state is entered the very first time
                    if (!sm_.checkState(State::Idle))
                        return false;
                    return digitalRead(Bit0) && digitalRead(Bit1);
                }

                Buffer &tx() {return tx_;}

                bool send()
                {
                    sm_.process(DataIsReadyForSending());
                    return sm_.checkState(State::RequestToSend);
                }

                void process()
                {
                    do
                    {
                        sm_.process(Process());
                    }
                    while (busyProcess_);
                }

            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}

                DATA_EVENT(Initialize);
                DATA_EVENT(DataIsReadyForSending);
                DATA_EVENT(Process);
                enum class State {Idle = 0, RequestToSend = 1, PeerIsPresent = 2, GetReady = 3, Sending = 4, AcknowledgeRequestToSend = 5, WaitForSilence = 6, Silence = 7, ReceivedMessage = 8, Error = 9};
                typedef gubg::StateMachine_ftop<DualRail<Receiver, Bit0, Bit1, Buffer>, State, State::Idle> SM;
                friend class gubg::StateMachine_ftop<DualRail<Receiver, Bit0, Bit1, Buffer>, State, State::Idle>;
                SM sm_;
                bool busyProcess_;
                void sm_exit(State s)
                {
#ifdef DR_DEBUG
                    leds_[(int)s].off();
#endif
                    switch (s)
                    {
                        case State::GetReady:
                            //GetReady pulse is over, pull Bit0-line down into a silence
                            digitalWrite(Bit0, false);
                            break;
                        case State::AcknowledgeRequestToSend:
                            //Release the Bit1-line, peer needs it for communication
                            pinMode(Bit1, INPUT_PULLUP);
                            break;
                    }
                }
                void sm_enter(typename SM::State &s)
                {
#ifdef DR_DEBUG
                    if (s() == State::Idle)
                    {
                        for (int i = 0; i < NrLEDs; ++i)
                            leds_[i].attach(22+2*i, 23+2*i);
                    }
                    leds_[(int)s()].on();
#endif
                    busyProcess_ = false;
                    switch (s())
                    {
                        case State::Idle:
                            //Release both lines with internal pull-ups enabled
                            pinMode(Bit0, INPUT_PULLUP);
                            pinMode(Bit1, INPUT_PULLUP);
                            rx_.clear();
                            rxBitIX_ = 0;
                            //delay(500);
                            break;

                        case State::RequestToSend:
                            //Take control of Bit0-line and pull down
                            digitalWrite(Bit0, false); pinMode(Bit0, OUTPUT); digitalWrite(Bit0, false);
                            txByteIX_ = 0;
                            txBitIX_ = 0;
                            break;
                        case State::PeerIsPresent:
                            //Peer pulled-down Bit1, indicating he is present
                            s.changeTo(State::GetReady);
                            break;
                        case State::GetReady:
                            //Take control of Bit1-line and pull down
                            digitalWrite(Bit1, false); pinMode(Bit1, OUTPUT); digitalWrite(Bit1, false);
                            //Bit0-line up to indicate to peer we saw its presence and have taken control of the Bit1-line
                            digitalWrite(Bit0, true);
                            s.changeTo(State::Sending);
                            break;
                        case State::Sending:
                            busyProcess_ = true;
                            break;

                        case State::AcknowledgeRequestToSend:
                            //Take control of Bit1-line and pull down to acknowledge the RequestToSend
                            digitalWrite(Bit1, false); pinMode(Bit1, OUTPUT); digitalWrite(Bit1, false);
                            busyProcess_ = true;
                            break;
                        case State::WaitForSilence:
                        case State::Silence:
                            busyProcess_ = true;
                            break;

                        case State::ReceivedMessage:
                            receiver_().dualrail_received(rx_);
                            s.changeTo(State::Idle);
                            break;

                        case State::Error:
                            delay(1000);
                            s.changeTo(State::Idle);
                            break;
                    }
                }
                void sm_event(typename SM::State &s, DataIsReadyForSending)
                {
                    switch (s())
                    {
                        case State::Idle:
                            if (!tx_.empty() && digitalRead(Bit0) && digitalRead(Bit1))
                                s.changeTo(State::RequestToSend);
                            break;
                    }
                }
                void sm_event(typename SM::State &s, Process)
                {
                    switch (s())
                    {
                        case State::Idle:
                            if (digitalRead(Bit0) == false)
                                //Peer pulled the Bit0-line down, he wants to start sending. To acknowledge
                                //this, we will pull down the Bit1-line
                                s.changeTo(State::AcknowledgeRequestToSend);
                            break;
                        case State::RequestToSend:
                            if (digitalRead(Bit1) == false)
                                s.changeTo(State::PeerIsPresent);
                            break;
                        case State::Sending:
                            {
                                const int line = (tx_[txByteIX_] & (1 << txBitIX_)) ? Bit1 : Bit0; 
                                
                                //We are still in silence
                                delay(1);

                                //Start the pulse
                                digitalWrite(line, true);
#ifdef DR_DEBUG
                                auto &led = leds_[(line == Bit0) ? 6 : 7];
                                led.on();
#endif
                                delay(1);
                                ++txBitIX_;
                                if (txBitIX_ == 8)
                                {
                                    ++txByteIX_;
                                    txBitIX_ = 0;
                                }
                                if (txByteIX_ == tx_.size())
                                    //Pulse will never end, other line will come high too
                                    s.changeTo(State::Idle);
                                else
                                {
                                    //Pulse is over
                                    digitalWrite(line, false);
#ifdef DR_DEBUG
                                    led.off();
#endif
                                }
                            }
                            break;

                        case State::AcknowledgeRequestToSend:
                            if (digitalRead(Bit0) == true)
                                //Bit0 went up, we have to release Bit1 asap, communication will start very soon now
                                s.changeTo(State::WaitForSilence);
                            break;
                        case State::WaitForSilence:
                        case State::Silence:
                            {
                                const bool b0 = digitalRead(Bit0);
                                const bool b1 = digitalRead(Bit1);
                                if (b0 && b1)
                                {
                                    //Both lines are up, we are idle again
                                    digitalWrite(13, false);
                                    if (s() == State::WaitForSilence)
                                    {
                                        if (rxBitIX_ != 0)
                                            //We currently only allow messages with a multiple of 8 bits
                                            s.changeTo(State::Error);
                                        else
                                            //We received a correct message
                                            s.changeTo(State::ReceivedMessage);
                                    }
                                    else
                                        //Oops, we missed the last bit, both lines should not go up together
                                        s.changeTo(State::Error);
                                }
                                else if (!b0 && !b1)
                                {
                                    //Both lines are down, this is a silence
                                    digitalWrite(13, false);
                                    s.changeTo(State::Silence);
                                }
                                else if (s() == State::Silence)
                                {
                                    //We were in silence and one line just went up: we received a bit
                                    s.changeTo(State::WaitForSilence);
                                    if (rxBitIX_ == 0)
                                        rx_.push_back(b1 ? 0x01 : 0x00);
                                    else if (b1)
                                        rx_.back() |= (1 << rxBitIX_);
                                    ++rxBitIX_;
                                    if (rxBitIX_ == 8)
                                        rxBitIX_ = 0;
                                    pinMode(13, OUTPUT);
                                    digitalWrite(13, b0);
                                }
                            }
                            break;
                    }
                }

                Buffer tx_;
                size_t txByteIX_ = 0;
                uint8_t txBitIX_ = 0;

                Buffer rx_;
                uint8_t rxBitIX_ = 0;

#ifdef DR_DEBUG
                static const int NrLEDs = 9;
                garf::LED leds_[NrLEDs];
#endif
        };
}

#endif
