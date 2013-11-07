#ifndef HEADER_garf_DualRail_hpp_ALREADY_INCLUDED
#define HEADER_garf_DualRail_hpp_ALREADY_INCLUDED

#include "gubg/StateMachine.hpp"

namespace garf
{
    enum class Role {Master, Slave};

    template <int Bit0, int Bit1, Role R>
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

                bool sendBits_block(const void *data, size_t nrBits)
                {
                    sm_.process(GrabLine());
                    if (!sm_.checkState(State::ReadyToSend))
                        return false;
                    uint8_t byte = 0;
                    //Does not matter which line we pull down the first time, the should both be low
                    bool bit = false;
                    for (size_t ix = 0; ix < nrBits; ++ix)
                    {
                        //Make sure the previous bit is pulled down again since there are more bits to send
                        digitalWrite((bit ? Bit1 : Bit0), false);
                        delayMicroseconds(1);

                        //Compute which line should pulse
                        const auto subix = ix%8;
                        if (subix == 0)
                            byte = ((const uint8_t*)data)[ix/8];
                        bit = (byte & (1 << subix));

                        //Set line to high; if this was the last bit, it has to stay high, else, the next round will pull it down again
                        digitalWrite((bit ? Bit1 : Bit0), true);
                        delayMicroseconds(1);
                    }
                    //Bring the other line high too
                    digitalWrite((bit ? Bit0 : Bit1), true);
                    sm_.process(ReleaseLine());
                    return true;
                }

                void process()
                {
                    sm_.process(Initialize());
                }

            private:
                DATA_EVENT(Initialize);
                DATA_EVENT(GrabLine);
                DATA_EVENT(ReleaseLine);
                enum class State {Idle, ReadyToSend};
                typedef gubg::StateMachine_ftop<DualRail<Bit0, Bit1, R>, State, State::Idle> SM;
                friend class gubg::StateMachine_ftop<DualRail<Bit0, Bit1, R>, State, State::Idle>;
                SM sm_;
                void sm_exit(State s) { }
                void sm_enter(typename SM::State &s)
                {
                    switch (s())
                    {
                        case State::Idle:
                            pinMode(Bit0, INPUT); digitalWrite(Bit0, true);
                            pinMode(Bit1, INPUT); digitalWrite(Bit1, true);
                            break;
                    }
                }
                void sm_event(typename SM::State &s, Initialize) {}
                void sm_event(typename SM::State &s, GrabLine)
                {
                    if (s() != State::Idle)
                        return;
                    if (!digitalRead(Bit0) || !digitalRead(Bit1))
                        //Too late, peer already Grabbed the line
                        return;
                    if (R == Role::Master)
                    {
                        pinMode(Bit0, OUTPUT); digitalWrite(Bit0, false);
                        while (!digitalRead(Bit1))
                        {
                            //Peer is trying to grab the line at the same time. But we are master, so peer should back-off
                        }
                        pinMode(Bit1, OUTPUT); digitalWrite(Bit1, false);
                    }
                    if (R == Role::Slave)
                    {
                        pinMode(Bit1, OUTPUT); digitalWrite(Bit1, false);
                        if (!digitalRead(Bit0))
                        {
                            //Peer is trying to grab the line at the same time. We are slave, so we should back-off
                            pinMode(Bit1, INPUT); digitalWrite(Bit1, true);
                            return;
                        }
                        pinMode(Bit0, OUTPUT); digitalWrite(Bit0, false);
                    }
                    s.changeTo(State::ReadyToSend);
                }
                void sm_event(typename SM::State &s, ReleaseLine)
                {
                    s.changeTo(State::Idle);
                }
        };
}

#endif
