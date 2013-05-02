#include "garf/Sonar.hpp"
#include "garf/Motor.hpp"
#include "garf/ObjectAvoidance.hpp"
#include "Arduino.h"

using namespace garf;

const int servo = 9;
const int totalDelay = 2000;
const int MaxSamples = 30;
int values[MaxSamples];
int nrSamples;

bool sonarMutex = false;
Sonar<7, 8, 200> sonar_l(sonarMutex);
Sonar<2, 4, 200> sonar_r(sonarMutex);
unsigned long distance_l, distance_r;

garf::Motor<6, 5> g_rightMotor;
garf::Motor<10, 9> g_leftMotor;

class OA: public ObjectAvoidance_crtp<OA>
{
    public:
        OA():
            target_l(), target_r(), current_l(), current_r(), pm_(millis()){}

        void oa_left(int speed) { target_l = speed/2; }
        void oa_right(int speed) { target_r = speed/2; }
        unsigned long oa_leftDistance(){return distance_l;}
        unsigned long oa_rightDistance(){return distance_r;}
        unsigned long oa_thresholdDistance(){return 18;}

        void process()
        {
            ObjectAvoidance_crtp::process();

            auto cm = millis();
            if (cm-pm_ > 5)
            {
                pm_ = cm;
                if (current_l < target_l)
                    ++current_l;
                else if (current_l > target_l)
                    current_l -= 2;
                if (current_r < target_r)
                    ++current_r;
                else if (current_r > target_r)
                    current_r -= 2;

                g_leftMotor.setSpeed(current_l);
                g_rightMotor.setSpeed(current_r);
            }
        }

    private:
       int target_l; 
       int target_r; 
       int current_l; 
       int current_r; 
       unsigned int pm_;
};
OA oa;

void setup()
{
    Serial.begin(9600);
    Serial.print("aoeuaoue");
    pinMode(13, OUTPUT);
    nrSamples = 0;
}

void loop()
{
    Serial.print((int)oa.debug_getState());
#if 1
    if (sonar_l.process(distance_l))
    {
    }
#endif
#if 1
    if (sonar_r.process(distance_r))
    {
    }
#endif
    oa.process();
}
