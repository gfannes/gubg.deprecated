#include "time.h"
#include <iostream>
#include <iomanip>

#include "timestamp.hpp"

using namespace std;

// Watch out, everything is zero-based, except for the year and day of month
bool TimeStamp::breakDown(bool absolute)
{
  if (absolute)
    {
      struct tm ltm;
#ifdef MINGW32
      ltm = *gmtime(&mTimeVal.tv_sec);
#else
      if (!gmtime_r(&mTimeVal.tv_sec, &ltm))
        {
          cerr << "Could not breakdown the timestamp" << endl;
          return false;
        }
#endif
      mBrokenTime.year = ltm.tm_year + 1900;
      mBrokenTime.month = ltm.tm_mon + 1;
      mBrokenTime.day = ltm.tm_mday;
      mBrokenTime.hour = ltm.tm_hour;
      mBrokenTime.minute = ltm.tm_min;
      mBrokenTime.second = ltm.tm_sec;
      mBrokenTime.microsecond = mTimeVal.tv_usec;
    }
  else
    {
      mBrokenTime.microsecond = mTimeVal.tv_usec;
      time_t nrLeft = mTimeVal.tv_sec;
      mBrokenTime.second = nrLeft%60;
      nrLeft /= 60;
      mBrokenTime.minute = nrLeft%60;
      nrLeft /= 60;
      mBrokenTime.hour = nrLeft%24;
      nrLeft /= 24;
      mBrokenTime.day = nrLeft%365;
      nrLeft /= 365;
      mBrokenTime.year = nrLeft;
      mBrokenTime.month = 0;
    }
  mBrokenTime.valid = true;
  mBrokenTime.absolute = absolute;
  return true;
}

ostream &operator<<(ostream &os, const TimeStamp &ts)
{
  if (ts.mBrokenTime.valid)
    {
      if (ts.mBrokenTime.absolute)
        os << ts.mBrokenTime.year << setw(2) << setfill('0') << ts.mBrokenTime.month << ts.mBrokenTime.day << ", " << ts.mBrokenTime.hour << ":" << ts.mBrokenTime.minute << ":" << ts.mBrokenTime.second << "." << ts.mBrokenTime.microsecond;
      else
        {
          if (ts.mBrokenTime.year > 0)
            os << ts.mBrokenTime.year << " years, ";
          if (ts.mBrokenTime.day > 0)
            os << ts.mBrokenTime.day << " days, ";
          if (ts.mBrokenTime.hour > 0)
            os << ts.mBrokenTime.hour << " hours, ";
          if (ts.mBrokenTime.minute > 0)
            os << ts.mBrokenTime.minute << " minutes, ";
          os << ts.mBrokenTime.second << " seconds, " << ts.mBrokenTime.microsecond << " microseconds";
        }
    }
  else
    os << ts.mTimeVal.tv_sec << " seconds, " << ts.mTimeVal.tv_usec << " microseconds";
}

