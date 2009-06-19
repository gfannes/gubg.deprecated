#ifndef timestamp_hpp
#define timestamp_hpp

#include <sys/time.h>
#include <iostream>

using namespace std;

class TimeStamp
{
public:

    TimeStamp(long seconds = 0, long microseconds = 0)
        {
            set(seconds, microseconds);
        }

    bool set(long seconds, long microseconds)
        {
            int nrSec = microseconds/1000000;
            microseconds -= nrSec*1000000;
            if (microseconds < 0)
            {
                --nrSec;
                microseconds += 1000000;
            }
            mTimeVal.tv_sec = seconds + nrSec;
            mTimeVal.tv_usec = microseconds;
            mBrokenTime.valid = false;
            return true;
        }
    long seconds() const{return mTimeVal.tv_sec;}
    long microSeconds() const{return mTimeVal.tv_usec;}
    bool now()
        {
            if (gettimeofday(&mTimeVal, NULL))
            {
                cerr << "Could not get the time" << endl;
                return false;
            }
            mBrokenTime.valid = false;
            return true;
        }
    bool add(const TimeStamp &offset)
        {
            if (mTimeVal.tv_usec + offset.mTimeVal.tv_usec > 1000000)
            {
                mTimeVal.tv_usec = mTimeVal.tv_usec + offset.mTimeVal.tv_usec - 1000000;
                mTimeVal.tv_sec++;
            } else
                mTimeVal.tv_usec += offset.mTimeVal.tv_usec;
            mTimeVal.tv_sec += offset.mTimeVal.tv_sec;
            mBrokenTime.valid = false;
            return true;
        }
    bool diff(const TimeStamp &rhs)
        {
            return set(seconds()-rhs.seconds(), microSeconds()-rhs.microSeconds());
        }
    bool breakDown(bool absolute = true);

    friend ostream &operator<<(ostream &os, const TimeStamp &ts);

private:
    typedef struct
    {
        bool valid;
        bool absolute;
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int microsecond;
    } BrokenTime;

    BrokenTime mBrokenTime;
    struct timeval mTimeVal;
};

#endif
