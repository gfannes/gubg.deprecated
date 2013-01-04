//@#platform.target:=arduino#@
#include "Arduino.h"
#include "Servo.h"

const int trigger = 7;
const int echo = 8;
const int servo = 9;
const int totalDelay = 2000;
Servo claw;
const int MaxSamples = 30;
int values[MaxSamples];
int nrSamples;

void setup()
{
    claw.attach(servo);
    pinMode(trigger, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(echo, LOW);
    nrSamples = 0;
}

void loop()
{
    digitalWrite(trigger, LOW);
    delay(10);
    //pulse
    digitalWrite(trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(trigger, LOW);
    //delayMicroseconds(5);

    int nr = pulseIn(echo, HIGH);
    nr /= 10;

    if (nr > 180)
        digitalWrite(13, HIGH);
    else
    {
        digitalWrite(13, LOW);

        values[nrSamples++] = nr;
        if (nrSamples >= MaxSamples)
        {
            nrSamples = 0;
            int sum = 0;
            for (auto i = 0; i < MaxSamples; i++)
                sum += values[i];
            claw.write(sum/MaxSamples);
        }
    }
}
