//@#platform.target:=arduino#@
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
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
    //Online is indicated with a blinking LED
    void oostatus_online() { g_blinker.set(true, true); }
    //Offline is indicated with a flat LED on
    void oostatus_offline() { g_blinker.set(false, true); }
};
OOStatus g_oostatus;

typedef gubg::FixedVector<ubyte, 8> Flips;
struct Decoder: gubg::d9::Decoder_crtp<Decoder, Flips>
{
    d9::ReturnCode d9_start()
    {
        MSS_BEGIN(d9::ReturnCode);
        g_oostatus.indicateOnline();
        MSS_END();
    }
    d9::ReturnCode d9_error(d9::ReturnCode)
    {
        MSS_BEGIN(d9::ReturnCode);
        MSS_END();
    }
    d9::ReturnCode d9_ubyte(ubyte)
    {
        MSS_BEGIN(d9::ReturnCode);
        MSS_END();
    }
};
Decoder g_decoder;

void setup()
{
    g_blinker.boot(20);
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
}
