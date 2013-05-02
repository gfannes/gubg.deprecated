#include "Arduino.h"

int i;

void setup()
{
    Serial.begin(9600);
    i = 0;
}

void loop()
{
    Serial.print("Hello world ");
    Serial.print(++i);
    Serial.print("\n\r");
}
