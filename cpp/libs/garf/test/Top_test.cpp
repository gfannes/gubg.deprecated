#include "garf/Elapser.hpp"
#include "garf/Top.hpp"

garf::Elapser g_elapser;

garf::top::Top<1000> g_top;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_elapser.process();
    g_top.process(g_elapser.elapse());
}
