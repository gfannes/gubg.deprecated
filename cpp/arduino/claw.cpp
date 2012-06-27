//@#platform.target:=arduino#@
#include "Arduino.h"
#include "Servo.h"

namespace 
{
    Servo claw;
    const int StartAngle = 60;
    const int StopAngle = 150;
    const int AngleStep = 1;
    int angle = StartAngle;
}

void setup()
{
    claw.attach(9);
}

void loop()
{
    for (; angle < StopAngle; angle += AngleStep)
    {
        claw.write(angle);
        delay(20);
    }
    for (; angle > StartAngle; angle -= AngleStep)
    {
        claw.write(angle);
        delay(20);
    }
}
