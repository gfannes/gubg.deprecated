//@#platform.target:=arduino#@
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "gubg/d9/Decoder.hpp"
#include "gubg/FixedVector.hpp"
using namespace gubg;

typedef unsigned char ubyte;

garf::Elapser g_elapser;

//This blinker is used to show the online/offline status
//We blink 10 times per second
garf::Blinker<100> g_blinker;

//We we don't receive indicateOnline() within 1 second, we will
//switch to offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    void oostatus_online() { g_blinker.set(garf::BlinkMode::Normal); }
    void oostatus_offline() { g_blinker.set(garf::BlinkMode::Flat); }
};
OOStatus g_oostatus;

typedef gubg::FixedVector<ubyte, 8> Flips;
struct Decoder: gubg::d9::Decoder_crtp<Decoder, Flips>
{
    enum State {Idle, Receiving, Received};

    State state;
    unsigned char motorValues_[4];
    unsigned char ix_;

    Decoder():
        state(Idle){}
    d9::ReturnCode d9_start()
    {
        MSS_BEGIN(d9::ReturnCode);
        state = Idle;
        MSS_END();
    }
    d9::ReturnCode d9_error(d9::ReturnCode)
    {
        MSS_BEGIN(d9::ReturnCode);
        MSS_END();
    }
    d9::ReturnCode d9_ubyte(ubyte b)
    {
        MSS_BEGIN(d9::ReturnCode);
        switch (b)
        {
            case 0xc0:
                //Nil => keep alive
                MSS(state == Idle, IllegalContent);
                //g_oostatus.indicateOnline();
                MSS(d9::ReturnCode::ContentComplete);
                break;
            case 0x94:
                //Array of length 4 => Motor values
                MSS(state == Idle, IllegalContent);
                state = Receiving;
                ix_ = 0;
                break;
            default:
                MSS(state == Receiving, IllegalContent);
                MSS(ix_ < 4, IllegalContent);
                motorValues_[ix_++];
                if (ix_ >= 4)
                {
                    state = Received;
                    MSS(d9::ReturnCode::ContentComplete);
                }
                break;
        }
        MSS_END();
    }
};
Decoder g_decoder;

void setup()
{
    g_blinker.set(garf::BlinkMode::Fast);
    garf::busyProcess<1000>(g_blinker);
    Serial.begin(9600);
    g_oostatus.setup();
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());

    if (Serial.available())
        g_decoder.process(Serial.read());

    if (g_decoder.state == Decoder::Received)
    {
        g_decoder.state = Decoder::Idle;
        g_oostatus.indicateOnline();
    }
}
