#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/Metronome.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;
typedef garf::Blinker<200> Blinker;
Blinker g_blinker;

class CycleMode: public garf::Metronome_crtp<CycleMode, 2000>
{
    public:
        CycleMode(): mode_(0) {}
        void metronome_tick()
        {
            switch (++mode_ % 3)
            {
                case 0: g_blinker.set(garf::BlinkMode::Flat); break;
                case 1: g_blinker.set(garf::BlinkMode::Normal); break;
                case 2: g_blinker.set(garf::BlinkMode::Fast); break;
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
