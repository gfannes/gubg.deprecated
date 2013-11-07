#include "Arduino.h"
#include "garf/DualRail.hpp"
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"

garf::DualRail<11, 12, garf::Role::Slave> g_dualRail;
garf::Elapser g_elapser;
garf::Blinker<100, 13> g_blinker;

void setup()
{
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_dualRail.process();
    g_blinker.setPattern(g_dualRail.isLineIdle() ? 0xf0 : 0xaa);
}
