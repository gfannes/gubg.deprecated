#include "garf/Sweeper.hpp"
#include "garf/Elapser.hpp"

typedef garf::Sweeper<2, 0, 180, 7> Sweeper;
Sweeper sweeper; 

garf::Elapser elapser;

void setup()
{
}

void loop()
{
    elapser.process();
    sweeper.process(elapser.elapse());
}
