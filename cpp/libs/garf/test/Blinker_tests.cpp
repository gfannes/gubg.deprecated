#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/Metronome.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;
typedef garf::Blinker<100> Blinker;
Blinker g_blinker;

class CycleMode: public garf::Metronome_crtp<CycleMode, 5000>
{
    public:
        CycleMode(): mode_(0) {}
        void metronome_tick()
        {
            switch (++mode_ % 4)
            {
                case 0: g_blinker.setPattern(0xaa); break;
                case 1: g_blinker.setPattern(0xcc); break;
                case 2: g_blinker.setPattern(0xf0); break;
                case 3: g_blinker.setPattern(0xfe); break;
            }
        }
    private:
        int mode_;
};
CycleMode g_cycleMode;

void setup()
{
}

void loop()
{
    g_elapser.process();
    g_cycleMode.process(g_elapser.elapse());
    g_blinker.process(g_elapser.elapse());
}
