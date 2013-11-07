#include "Arduino.h"
#include "garf/DualRail.hpp"

garf::DualRail<11, 12, garf::Role::Master> g_dualRail;

void setup()
{
}

void loop()
{
    g_dualRail.process();
    g_dualRail.sendBits_block("\xc0\x5a", 16);
    delayMicroseconds(10);
}
