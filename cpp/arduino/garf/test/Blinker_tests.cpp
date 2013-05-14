#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;
typedef garf::Blinker<200> Blinker;
Blinker g_blinker;

void setup()
{
    //This necessary since arduino does not run the ctors for globals
    g_elapser = Elapser();
    g_blinker = Blinker();
    g_blinker.set(garf::BlinkMode::Normal);
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
}
