#include "garf/Sonar.hpp"
#include "Arduino.h"

using namespace garf;

const int trigger = 7;
const int echo = 8;

Sonar<7, 8, 200> sonar;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int s = (int)sonar.debug_getState();
#if 1
    switch (s)
    {
        case 1: break;
        case 2:
                Serial.print("\n\rSend pulse");
                break;
        default:
                Serial.print(" ");
                Serial.print((int)sonar.debug_getState());
                break;
    }
#endif
    unsigned long cm;
    if (sonar.process(cm))
    {
#if 0
        Serial.print(" >> ");
#else
        Serial.print("\n\r");
#endif
        Serial.print(cm);
    }
}
