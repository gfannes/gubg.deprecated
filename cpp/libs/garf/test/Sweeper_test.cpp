#include "garf/Sweeper.hpp"
#include "garf/ProcessInfo.hpp"
#include "garf/Elapser.hpp"

typedef garf::Sweeper<2, 0, 180, 7> Sweeper;
Sweeper sweeper; 

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
    sweeper.process(elapser.elapse());
    pi.process(elapser.elapse());
}
