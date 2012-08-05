//@#platform.target:=arduino#@
#include "Arduino.h"

namespace 
{
    const int LED = 13;
    const int Max = 5;
}

void setup()
{
    pinMode(LED, OUTPUT);
}

void loop()
{
    for (char i = 0; i < Max; ++i)
    {
        digitalWrite(LED, HIGH);
        delay(i*100);
        digitalWrite(LED, LOW);
        delay(i*100);
    }
    for (char i = Max; i >= 0; --i)
    {
        digitalWrite(LED, HIGH);
        delay(i*100);
        digitalWrite(LED, LOW);
        delay(i*100);
    }
}
