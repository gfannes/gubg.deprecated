#ifndef HEADER_garf_DualRail_hpp_ALREADY_INCLUDED
#define HEADER_garf_DualRail_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"

namespace garf
{
    enum class Role {Master, Slave};

    template <int Bit0, int Bit1, typename Buffer>
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

                Buffer &buffer() {return buffer_;}

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
                    while (sm_.checkState(State::Sending));
                }

            private:
                DATA_EVENT(Initialize);
                DATA_EVENT(DataIsReadyForSending);
                DATA_EVENT(Process);
                enum class State {Idle, RequestToSend, PeerIsPresent, GetReady, Sending};
                typedef gubg::StateMachine_ftop<DualRail<Bit0, Bit1, Buffer>, State, State::Idle> SM;
                friend class gubg::StateMachine_ftop<DualRail<Bit0, Bit1, Buffer>, State, State::Idle>;
                SM sm_;
                void sm_exit(State s)
                {
                    switch (s)
                    {
                        case State::GetReady:
                            //Take control of Bit1-line and pull down
                            digitalWrite(Bit1, false); pinMode(Bit1, OUTPUT); digitalWrite(Bit1, false);
                            //Pull down Bit0 too
                            digitalWrite(Bit0, false);
                            break;
                    }
                }
                void sm_enter(typename SM::State &s)
                {
                    switch (s())
                    {
                        case State::Idle:
                            //Release both lines with internal pull-ups enabled
                            pinMode(Bit0, INPUT); digitalWrite(Bit0, true);
                            pinMode(Bit1, INPUT); digitalWrite(Bit1, true);
                            break;
                        case State::RequestToSend:
                            //Take control of Bit0-line and pull down
                            digitalWrite(Bit0, false); pinMode(Bit0, OUTPUT); digitalWrite(Bit0, false);
                            byteIX_ = 0;
                            bitIX_ = 0;
                            break;
                        case State::PeerIsPresent:
                            //Peer pulled-down Bit1, indicating he is present
                            s.changeTo(State::GetReady);
                            break;
                        case State::GetReady:
                            //Bit0-line up to indicate to peer we saw its presence and are about to take control of Bit1-line
                            //too and will start sending
                            digitalWrite(Bit0, true);
                            s.changeTo(State::Sending);
                            break;
                        case State::Sending:
                            break;
                    }
                }
                void sm_event(typename SM::State &s, DataIsReadyForSending)
                {
                    switch (s())
                    {
                        case State::Idle:
                            if (!buffer_.empty())
                                s.changeTo(State::RequestToSend);
                            break;
                    }
                }
                void sm_event(typename SM::State &s, Process)
                {
                    switch (s())
                    {
                        case State::RequestToSend:
                            if (digitalRead(Bit1) == false)
                                s.changeTo(State::PeerIsPresent);
                            break;
                        case State::Sending:
                            {
                                const int line = (buffer_[byteIX_] & (1 << bitIX_)) ? Bit1 : Bit0; 
                                digitalWrite(line, true);
                                ++bitIX_;
                                if (bitIX_ == 8)
                                {
                                    ++byteIX_;
                                    bitIX_ = 0;
                                }
                                digitalWrite(line, false);
                                if (byteIX_ == buffer_.size())
                                    s.changeTo(State::Idle);
                            }
                            break;
                    }
                }

                Buffer buffer_;
                size_t byteIX_ = 0;
                uint8_t bitIX_ = 0;
        };
}

#endif
