#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "garf/Motor.hpp"
#include "garf/Average.hpp"

typedef unsigned char ubyte;

typedef garf::Elapser Elapser;
Elapser g_elapser;

//This blinker is used to show the online/offline status
//We blink 10 times per second
typedef garf::Blinker<100> Blinker;
Blinker g_blinker;

//If we don't receive indicateOnline() within 1 second, we go offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    void oostatus_online()
    {
        g_blinker.setPattern(0xf0);
    }
    void oostatus_offline()
    {
        g_blinker.setPattern(0xaa);
    }
};
OOStatus g_oostatus;

void setup()
{
    g_elapser = Elapser();
    g_blinker.setPattern(0x01);
    garf::busyProcess<1000>(g_blinker);
    Serial.begin(9600);
    g_oostatus.setup();
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());

    if (Serial.available())
    {
        g_oostatus.indicateOnline();
        Serial.write(Serial.read());
    }
}
