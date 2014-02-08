#include "garf/Sonar.hpp"
#include "garf/Sweeper.hpp"
#include "garf/Elapser.hpp"
#include "garf/Motor.hpp"
#include "garf/ObjectAvoidance.hpp"
#include "garf/IRSensor.hpp"
#include "Arduino.h"
using namespace garf;

const int servo = 9;
const int totalDelay = 2000;
const int MaxSamples = 30;
int values[MaxSamples];
int nrSamples;

const int echo = 8;
typedef garf::Sonar<2, echo, 200> Sonar_left;
typedef garf::Sonar<7, echo, 200> Sonar_right;
bool mutex = false;
Sonar_left  sonar_left(mutex);
Sonar_right sonar_right(mutex);
volatile unsigned long distance_l, distance_r;

typedef garf::Sweeper<30, 20, 110, 5> SweeperR;
typedef garf::Sweeper<31, 20, 110, 5> SweeperL;
SweeperR sweeperr; 
SweeperL sweeperl;

garf::Motor<10, 9> g_rightMotor;
garf::Motor<5, 6> g_leftMotor;

garf::Elapser g_elapser;


const auto scaler = 10;
const auto offset = 10;
const auto speedscaler = 2;

class OA: public ObjectAvoidance_crtp<OA>
{
    public:
        OA():
            target_l(), target_r(), current_l(), current_r(), pm_(millis()){}

        void oa_left(int speed) { target_l = speed*0.7; }
        void oa_right(int speed) { target_r = speed*0.7; }
        bool oa_detectedLeft(){return distance_l <= (sweeperl.position() / scaler + offset);}
        bool oa_detectedRight(){return distance_r <= ((sweeperr.range() - sweeperr.position()) / scaler + offset);}

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

class IR: public IRSensor<IR, 42>
{
    public:
        void irsensor_detect(bool b)
        {
            //Serial.println(b ? "Stoppen!" : "OK to go!");
        }
};
IR ir;

void setup()
{
    Serial.begin(9600);
    sonar_left.init();
    sonar_right.init();
    pinMode(13, OUTPUT);
    ir.init();
    oa.init();
    nrSamples = 0;
}

void loop()
{
    g_elapser.process();

    g_rightMotor.process(g_elapser.elapse());
    g_leftMotor.process(g_elapser.elapse());

    sweeperr.process(g_elapser.elapse());
    sweeperl.process(g_elapser.elapse());

#if 1
    unsigned long cm;
    if (sonar_left.process(cm))
    {
        Serial.print("\r\nleft  ");
        Serial.print(cm);
        distance_l = cm;
    }
    if (sonar_right.process(cm))
    {
        Serial.print("\r\nright ");
        Serial.print(cm);
        distance_r = cm;
    }
#endif

    oa.process();
}
