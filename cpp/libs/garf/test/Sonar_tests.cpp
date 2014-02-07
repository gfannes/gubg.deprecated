#include "garf/Sonar.hpp"
#include "Arduino.h"

const int echo = 8;
typedef garf::Sonar<2, echo, 200> Sonar_left;
typedef garf::Sonar<7, echo, 200> Sonar_right;
bool mutex;
Sonar_left  sonar_left(mutex);
Sonar_right sonar_right(mutex);

void setup()
{
    mutex = false;
    sonar_left.init();
    sonar_right.init();
    Serial.begin(9600);
}

void loop()
{
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
}
