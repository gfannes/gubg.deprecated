#include <iostream>
#include <fstream>
#include <thread>
using namespace std;
#define L(m) cout<<m<<endl

namespace 
{
    enum Mode {Poll, Wait, MotorValues};
}

int main()
{
    ofstream of("/dev/ttyACM0");
    for (size_t i = 0; ; ++i)
    {
        Mode mode = Poll;
        if (i%10==0)
            mode = Wait;
        if (i%7==0)
            mode = MotorValues;
        char ch = '.';
        switch (mode)
        {
            case Poll:
                ch = '.';
                of << "\xd9\xc0";
                break;
            case Wait:
                ch = '-';
                this_thread::sleep_for(chrono::milliseconds(1500));
                break;
            case MotorValues:
                ch = '*';
                of << "\x94\x01\x01\x02\x03";
                break;
        }
        cout << ch;
        cout.flush();
        of.flush();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    return 0;
}
