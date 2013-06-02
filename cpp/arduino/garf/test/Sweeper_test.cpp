#include "garf/Sweeper.hpp"
#include "garf/Global.hpp"


typedef garf::Sweeper<2, 0, 180, 7> Sweeper_;
typedef garf::Global<Sweeper_> Sweeper;
Sweeper sweeper; 

void setup()
{
    //This is optional with Global
    //sweeper.init();
}

void loop()
{
    sweeper->process();
}
