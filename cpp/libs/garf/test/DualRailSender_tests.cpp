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
    void process(unsigned int elapse)
    {
        DualRail::process();
        Metronome_crtp::process(elapse);
    }
    void metronome_tick()
    {
        DualRail::send("a", 1);
    }
    void dualrail_received(const my::String &) {}
};
Sender sender;

garf::Elapser elapser;

void setup()
{
    Serial.begin(9600);
    Serial.write("Starting\n\r");
}

void loop()
{
    elapser.process();
    pinMode(13, OUTPUT);
    digitalWrite(13, true);
    sender.process(elapser.elapse());
    digitalWrite(13, false);
}

