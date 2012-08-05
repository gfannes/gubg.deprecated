//@#platform.target:=arduino#@
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "gubg/d9/Decoder.hpp"
#include "gubg/FixedVector.hpp"

typedef unsigned char ubyte;

garf::Elapser g_elapser;

//This blinker is used to show the online/offline status
//We blink 10 times per second
garf::Blinker<100> g_blinker;

//We we don't receive indicateOnline() within 1 second, we will
//switch to offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    //Online is indicated with a flat LED on
    void oostatus_online() { g_blinker.set(false, true); }
    //Offline is indicated with a blinking LED
    void oostatus_offline() { g_blinker.set(true, true); }
};
OOStatus g_oostatus;

typedef gubg::FixedVector<ubyte, 8> Flips;
struct Decoder: gubg::d9::Decoder_crtp<Decoder, Flips>
{
};
Decoder g_decoder;

void setup()
{
    g_oostatus.setup();
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());

    //We switch a few times to online to test things
    if (millis() == 3000)
        g_oostatus.indicateOnline();
    if (5000 <= millis() && millis() <= 10000)
        g_oostatus.indicateOnline();
}
