//@#platform.target:=arduino#@
#include "Arduino.h"
#include "Servo.h"

namespace 
{
    Servo claw;
    const int StartAngle = 0;
    const int StopAngle = 180;
    const int AngleStep = 1;
    int angle;
    int display;
}

void setup()
{
    pinMode(13, OUTPUT);
    claw.attach(9);
    angle = StartAngle;
    display = 0;
}

void loop()
{
    if (display)
        digitalWrite(13, HIGH);
    else
        digitalWrite(13, LOW);
    display = 1-display;

    for (; angle < StopAngle; angle += AngleStep)
    {
        claw.write(angle);
        delay(8);
    }
    for (; angle > StartAngle; angle -= AngleStep)
    {
        claw.write(angle);
        delay(20);
    }
}
