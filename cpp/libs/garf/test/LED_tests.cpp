#include "garf/LED.hpp"
#include "garf/Metronome.hpp"
#include "garf/Elapser.hpp"

struct NightRider: garf::Metronome_crtp<NightRider, 100>
{
    static const int NrLEDs = 8;
    void metronome_tick()
    {
        if (ix_ == -1)
        {
            for (int i = 0; i < NrLEDs; ++i)
                leds[i].attach(22+2*i, 23+2*i);
            ix_ = 0;
        }

        leds[ix_].off();
        ++ix_;
        if (ix_ == NrLEDs)
            ix_ = 0;
        leds[ix_].on();
    }
    garf::LED leds[NrLEDs];
    int ix_ = -1;
};
NightRider nightRider;
garf::Elapser elapser;

void setup()
{
}
void loop()
{
    elapser.process();
    nightRider.process(elapser.elapse());
}
