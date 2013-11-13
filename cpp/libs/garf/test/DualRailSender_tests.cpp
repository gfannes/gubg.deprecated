#include "Arduino.h"
#define DR_DEBUG
#include "garf/DualRail.hpp"
#include "garf/Metronome.hpp"
#include "garf/Elapser.hpp"
#include "gubg/FixedVector.hpp"

namespace my
{
    typedef gubg::FixedVector<uint8_t, 20> String;
}

struct Sender: garf::DualRail<Sender, 11, 12, my::String>, garf::Metronome_crtp<Sender, 100>
{
    Sender()
    {
        const char *msg = "Hello world!";
        for (int i = 0; i < 12; ++i)
            DualRail::tx().push_back(msg[i]);
        //for (; msg != '\0'; ++msg)
            //DualRail::tx().push_back(*(const uint8_t*)msg);
    }
    void process(unsigned int elapse)
    {
        DualRail::process();
        Metronome_crtp::process(elapse);
    }
    void metronome_tick()
    {
        DualRail::send();
    }
    void dualrail_received(const my::String &) {}
};
Sender sender;

garf::Elapser elapser;

void setup()
{
}

void loop()
{
    elapser.process();
    pinMode(13, OUTPUT);
    digitalWrite(13, true);
    sender.process(elapser.elapse());
    digitalWrite(13, false);
}
