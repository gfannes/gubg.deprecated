#include "garf/Motor.hpp"
#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/Metronome.hpp"

class Motor: public garf::Metronome_crtp<Motor, 100>
{
    public:
        void metronome_tick()
        {
            speed_ += delta_;
            if (speed_ > 255)
                delta_ = -Delta;
            else if (speed_ < -255)
                delta_ = Delta;
            motor.setSpeed(speed_);
            motor.process(0);
        }

    private:
        //garf::Motor<5, 6> motor;
        //garf::Motor<10, 11> motor;
        garf::Motor<9, 10> motor;
        int speed_ = 0;
        static const int Delta = 4;
        int delta_ = Delta;
};
Motor g_motor;

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
    g_motor.process(g_elapser.elapse());
}
