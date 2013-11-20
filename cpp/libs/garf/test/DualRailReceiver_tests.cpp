#include "Arduino.h"
#define DR_DEBUG
#include "garf/DualRail.hpp"
#include "garf/Elapser.hpp"
#include "gubg/FixedVector.hpp"

namespace my
{
    typedef gubg::FixedVector<uint8_t, 20> String;
}

struct Receiver: public garf::DualRail<Receiver, 11, 12, my::String>
{
    void dualrail_received(const my::String &msg)
    {
        for (auto ch: msg)
            Serial.write((char)ch);
        Serial.write('\n');
    }
};
Receiver receiver;

garf::Elapser elapser;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    elapser.process();
    receiver.process();
}
