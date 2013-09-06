#include "garf/Elapser.hpp"
#include "garf/Metronome.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;

class Ticker: public garf::Metronome_crtp<Ticker, 1000>
{
    public:
        void metronome_tick()
        {
            Serial.write('*');
        }
    private:
};
Ticker g_ticker;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_elapser.process();
    g_ticker.process(g_elapser.elapse());
}
