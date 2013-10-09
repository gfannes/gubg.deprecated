#include "garf/Sweeper.hpp"
#include "garf/ProcessInfo.hpp"
#include "garf/Elapser.hpp"
#include "garf/Sonars.hpp"

typedef garf::Sweeper<30, 0, 100, 10> SweeperR;
typedef garf::Sweeper<31, 50, 150, 10 > SweeperL;
SweeperR sweeperr; 
SweeperL sweeperl; 

typedef garf::Sonars<7, 2, 8, 10> S;
S sonars;
S::Which wh;

class ProcessInfo: public garf::ProcessInfo_crtp<ProcessInfo>
{
    public:
        void pi_count(long c)
        {
            Serial.print(c);
            Serial.println(" ");
        }
    private:
};
ProcessInfo pi;

garf::Elapser elapser;

void setup()
{
    Serial.begin(9600);
    sonars.init();
}

void loop()
{
    elapser.process();
    sweeperr.process(elapser.elapse());
    sweeperl.process(elapser.elapse());
    pi.process(elapser.elapse());
    unsigned long distance;
    if (sonars.process(wh, distance))
    {
        switch (wh)
        {
            case S::Which::Left:
                distance = distance;
                Serial.print(distance);
                Serial.println("");
                break;
            case S::Which::Right:
                distance = distance;
/*
                Serial.print("               ");
                Serial.print(distance);
                Serial.println("");
                */
                break;
        }
    }
}
