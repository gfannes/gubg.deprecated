#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "Arduino.h"

typedef garf::Elapser Elapser;
Elapser g_elapser;
typedef garf::Blinker<100> Blinker;
Blinker g_blinker;

int i;

void setup()
{
    g_elapser = Elapser();
    g_blinker.setPattern(0xf0);
    Serial.begin(9600);
    i = 0;
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    Serial.print("Hello world ");
    Serial.print(++i);
    Serial.print("\n\r");
    delay(100);
}
