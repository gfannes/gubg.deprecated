#include "Messagetypes.h"
#include "gubg/file/File.hpp"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

namespace 
{
    const string name = gubg::file::File(__FILE__).basename();
    const string homeColor = "ff0000";
    const string awayColor = "0000ff";
    const string log = name+".log";

    ofstream fo;

    void readMessage()
    {
        string str;
        cin >> str;
        fo << str << endl;
    }
}

int main()
{
    cout << "RobotOption " << USE_NON_BLOCKING << " " << 0 << endl;

    string cmd;
    int seq;
    cin >> cmd >> seq;
    if (seq == 1)
    {
        cout << "Name " << name << endl;
        cout << "Colour " << homeColor << " " << awayColor << endl;
    }

    fo.open(log, ios_base::out | ios_base::trunc);
    readMessage();
    readMessage();
    readMessage();

    cout << "Rotate 2 1" << endl;

    while (true)
    {
        readMessage();
        cout << "Shoot 1" << endl;
    }

    return 0;
}
