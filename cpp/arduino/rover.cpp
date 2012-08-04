//@#platform.target:=arduino#@
#include "garf/Status.hpp"

garf::Status g_status;

void setup()
{
}

void loop()
{
    g_status.process();
}
