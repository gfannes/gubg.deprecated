#include "gubg/Strange.hpp"
#include <iostream>
using namespace std;
using namespace gubg;

bool popTime(double &time, Strange &strange)
{
    Strange num;
    if (!strange.popUntil(num, ' ') && !strange.popAll(num))
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
    double total = 0;
    string line;
    while (true)
    {
        if (!getline(cin, line))
            return -1;
        Strange strange(line);
        if (strange.popCharIf('t'))
        {
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
