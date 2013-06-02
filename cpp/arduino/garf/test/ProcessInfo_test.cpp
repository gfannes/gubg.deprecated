#include "garf/ProcessInfo.hpp"
#include "garf/Elapser.hpp"

class ProcessInfo: public garf::ProcessInfo_crtp<ProcessInfo>
{
    public:
        void pi_count(long c)
        {
            Serial.print(c);
            Serial.println(" ");
        }
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
    pi.process(elapser.elapse());
}
