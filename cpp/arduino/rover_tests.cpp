#include <iostream>
#include <fstream>
#include <thread>
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    ofstream of("/dev/ttyACM0");
    for (size_t i = 0; ; ++i)
    {
        const bool doWait = (i%10==0);
        cout << (doWait ? '*' : '.');
        cout.flush();
        of << "\xd9\xc0";
        of.flush();
        this_thread::sleep_for(chrono::milliseconds((doWait ? 2000 : 500)));
    }
    return 0;
}
