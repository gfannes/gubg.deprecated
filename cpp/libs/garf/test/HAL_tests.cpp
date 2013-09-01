#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "Arduino.h"

typedef garf::Elapser Elapser;
Elapser g_elapser;
typedef garf::Blinker<200> Blinker;
Blinker g_blinker;

int i;

void setup()
{
    g_elapser = Elapser();
    g_blinker = Blinker();
    g_blinker.set(garf::BlinkMode::Normal);
    Serial.begin(9600);
    i = 0;
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());

    if (Serial.available())
    {
        char ch = Serial.read() - 1;
        Serial.print(ch);
    }
}
