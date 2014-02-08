#include "garf/Motor.hpp"
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/Metronome.hpp"

class Motors: public garf::Metronome_crtp<Motors, 100>
{
    public:
        void metronome_tick()
        {
            speed_ += delta_;
            if (speed_ > 255)
                delta_ = -Delta;
            else if (speed_ < -255)
                delta_ = Delta;
            motor_l.setSpeed(speed_/2);
            motor_r.setSpeed(speed_/2);
            motor_l.process(0);
            motor_r.process(0);
        }

    private:
        garf::Motor<10, 9> motor_r;
        garf::Motor<5, 6> motor_l;
        int speed_ = 0;
        static const int Delta = 4;
        int delta_ = Delta;
};
Motors g_motors;

garf::Elapser g_elapser;

garf::Blinker<100> g_blinker;

void setup()
{
    g_blinker.setPattern(0xf0);
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_motors.process(g_elapser.elapse());
}
