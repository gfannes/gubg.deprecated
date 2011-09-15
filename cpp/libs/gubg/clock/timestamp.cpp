#include "time.h"
#include "timestamp.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

// Watch out, everything is zero-based, except for the year and day of month
bool TimeStamp::breakDown(bool absolute)
{
    if (absolute)
    {
        struct tm ltm;
#ifdef MINGW32
        ltm = *gmtime(&timeVal_.tv_sec);
#else
        if (!gmtime_r(&timeVal_.tv_sec, &ltm))
        {
            cerr << "Could not breakdown the timestamp" << endl;
            return false;
        }
#endif
        brokenTime_.year = ltm.tm_year + 1900;
        brokenTime_.month = ltm.tm_mon + 1;
        brokenTime_.day = ltm.tm_mday;
        brokenTime_.hour = ltm.tm_hour;
        brokenTime_.minute = ltm.tm_min;
        brokenTime_.second = ltm.tm_sec;
        brokenTime_.microsecond = timeVal_.tv_usec;
    }
    else
    {
        brokenTime_.microsecond = timeVal_.tv_usec;
        time_t nrLeft = timeVal_.tv_sec;
        brokenTime_.second = nrLeft%60;
        nrLeft /= 60;
        brokenTime_.minute = nrLeft%60;
        nrLeft /= 60;
        brokenTime_.hour = nrLeft%24;
        nrLeft /= 24;
        brokenTime_.day = nrLeft%365;
        nrLeft /= 365;
        brokenTime_.year = nrLeft;
        brokenTime_.month = 0;
    }
    brokenTime_.valid = true;
    brokenTime_.absolute = absolute;
    return true;
}

ostream &operator<<(ostream &os, const TimeStamp &ts)
{
    if (ts.brokenTime_.valid)
    {
        if (ts.brokenTime_.absolute)
            os << ts.brokenTime_.year << setw(2) << setfill('0') << ts.brokenTime_.month << ts.brokenTime_.day << ", " << ts.brokenTime_.hour << ":" << ts.brokenTime_.minute << ":" << ts.brokenTime_.second << "." << ts.brokenTime_.microsecond;
        else
        {
            if (ts.brokenTime_.year > 0)
                os << ts.brokenTime_.year << " years, ";
            if (ts.brokenTime_.day > 0)
                os << ts.brokenTime_.day << " days, ";
            if (ts.brokenTime_.hour > 0)
                os << ts.brokenTime_.hour << " hours, ";
            if (ts.brokenTime_.minute > 0)
                os << ts.brokenTime_.minute << " minutes, ";
            os << ts.brokenTime_.second << " seconds, " << ts.brokenTime_.microsecond << " microseconds";
        }
    }
    else
        os << ts.timeVal_.tv_sec << " seconds, " << ts.timeVal_.tv_usec << " microseconds";
}

