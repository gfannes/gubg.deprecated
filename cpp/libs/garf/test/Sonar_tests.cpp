#include "garf/Sonar.hpp"
#include "garf/Sweeper.hpp"
#include "garf/Elapser.hpp"
#include "Arduino.h"

const int echo = 8;
typedef garf::Sonar<2, echo, 200> Sonar_left;
typedef garf::Sonar<7, echo, 200> Sonar_right;
bool mutex = false;
Sonar_left  sonar_left(mutex);
Sonar_right sonar_right(mutex);

typedef garf::Sweeper<30, 30, 130, 10> SweeperR;
typedef garf::Sweeper<31, 50, 150, 10 > SweeperL;
SweeperR sweeperr; 
SweeperL sweeperl;

garf::Elapser elapser;

void setup()
{
    sonar_left.init();
    sonar_right.init();
    Serial.begin(9600);
}

void loop()
{
    elapser.process();
    sweeperr.process(elapser.elapse());
    sweeperl.process(elapser.elapse());

#if 1
    unsigned long cm;
    if (sonar_left.process(cm))
    {
        Serial.print("\n\rleft  ");
        Serial.print(cm);
    }
    if (sonar_right.process(cm))
    {
        Serial.print("\n\rright ");
        Serial.print(cm);
    }
#endif
}
