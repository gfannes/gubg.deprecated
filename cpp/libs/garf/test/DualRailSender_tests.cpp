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

struct Sender: garf::Metronome_crtp<Sender, 1000>
{
    Sender()
    {
        dr_.tx().push_back(0xc0);
        dr_.tx().push_back(0x5a);
    }
    void process(unsigned int elapse)
    {
        dr_.process();
        Metronome_crtp::process(elapse);
    }
    void metronome_tick()
    {
        dr_.send();
    }
    garf::DualRail<11, 12, my::String> dr_;
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
    delay(1);
}
