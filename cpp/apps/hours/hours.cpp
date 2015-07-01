#include "gubg/Strange.hpp"
#include <iostream>
using namespace std;
using namespace gubg;

void print_help()
{
    cout << "New day is started with 't' or '+'" << endl;
    cout << "Numbers can be separated with ' ' or '/'" << endl;
}

bool popTime(double &time, Strange &strange)
{
    Strange num;
    //Numbers can be separated by ' ' or '/'
    if (!strange.popUntil(num, ' ') && !strange.popUntil(num, '/') &&!strange.popAll(num))
        return false;
    if (num.contains('.'))
    {
        if (!num.popFloat(time))
            return false;
    }
    else
    {
        int min;
        if (!num.popDecimal(min))
            return false;
        if (min < 10)
            //We assume time was given in hours
            time = min;
        else
            //We assume time is given in army
            time = min/100 + (min%100)/60.0;
    }
    return true;
}

int main()
{
    print_help();
    double total = 0;
    string line;
    while (true)
    {
        if (!getline(cin, line))
            return -1;
        Strange strange(line);
        //New day is started with 't' or '+'
        if (strange.popCharIf('t') || strange.popCharIf('+'))
        {
            //New day: read the start, lunch and stop to compute the total hours worked
            strange.strip(' ');
            double start, lunch, stop;
            if (!popTime(start, strange) || !popTime(lunch, strange) || !popTime(stop, strange))
            {
                cout << "ERROR::You have to give <start> <lunch> <stop>" << endl;
                continue;
            }
            total = stop-start-lunch;
            cout << "Total hours: " << total << endl;
        }
        else
        {
            double t = 0;
            double tmp;
            while (popTime(tmp, strange))
                t += tmp;
            cout << "Remaining: " << total-t << endl;
        }
    }
    return 0;
}
