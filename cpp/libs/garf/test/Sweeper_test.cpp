#include "garf/Sweeper.hpp"
#include "garf/ProcessInfo.hpp"
#include "garf/Elapser.hpp"

typedef garf::Sweeper<30, 0, 100, 7> SweeperR;
typedef garf::Sweeper<31, 80, 180, 7 > SweeperL;
SweeperR sweeperr; 
SweeperL sweeperl; 

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
}

void loop()
{
    elapser.process();
    sweeperr.process(elapser.elapse());
    sweeperl.process(elapser.elapse());
    pi.process(elapser.elapse());
}
