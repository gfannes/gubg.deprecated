#include "Arduino.h"
#include "Servo/Servo.h"

class Arm: public Servo
{
    public:
        static const int Pickup = 60;
        static const int Check = 90;
        static const int Green = 120;
        static const int Red = 150;

        void init()
        {
            attach(9);
            write(Pickup);
        }
        void gotoPickup()
        {
            write(Pickup);
            delay(500);
        }
        void gotoCheck()
        {
            write(Check);
            delay(500);
        }
        void gotoPlace(int place)
        {
            write(place);
            delay(500);
        }
};
Arm arm;

class Needle: public Servo
{
    public:
        const int Down = 0;
        const int Middle = 40;
        const int Up = 120;

        void init()
        {
            attach(10);
            write(Up);
        }
        void down()
        {
            write(Down);
            delay(200);
        }
        void middle()
        {
            write(Middle);
            delay(200);
        }
        void up()
        {
            write(Up);
            delay(200);
        }
};
Needle needle;

void setup()
{
    arm.init();
    needle.init();
}

void pickup()
{
    arm.gotoPickup();
    needle.middle();
    delay(2000);
    needle.up();
}
int checkColor()
{
    arm.gotoCheck();
    delay(3000);
    return Arm::Red;
}
void dropInBox(int place)
{
    arm.gotoPlace(place);
    needle.down();
    delay(500);
    needle.up();
}

void loop()
{
    pickup();
    auto dropPlace = checkColor();
    dropInBox(dropPlace);
}
