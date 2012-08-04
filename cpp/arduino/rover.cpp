//@#platform.target:=arduino#@
#include "garf/Elapser.hpp"
#include "garf/Status.hpp"

garf::Elapser g_elapser;
garf::Status<100> g_status;

void setup()
{
}

void loop()
{
    g_elapser.process();
    g_status.process(g_elapser.elapse());
}
