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
            timeVal_.tv_sec = seconds + nrSec;
            timeVal_.tv_usec = microseconds;
            brokenTime_.valid = false;
            return true;
        }
        long seconds() const{return timeVal_.tv_sec;}
        long microSeconds() const{return timeVal_.tv_usec;}
        bool now()
        {
            if (gettimeofday(&timeVal_, NULL))
            {
                std::cerr << "Could not get the time" << std::endl;
                return false;
            }
            brokenTime_.valid = false;
            return true;
        }
        bool add(const TimeStamp &offset)
        {
            if (timeVal_.tv_usec + offset.timeVal_.tv_usec > 1000000)
            {
                timeVal_.tv_usec = timeVal_.tv_usec + offset.timeVal_.tv_usec - 1000000;
                timeVal_.tv_sec++;
            } else
                timeVal_.tv_usec += offset.timeVal_.tv_usec;
            timeVal_.tv_sec += offset.timeVal_.tv_sec;
            brokenTime_.valid = false;
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

        BrokenTime brokenTime_;
        struct timeval timeVal_;
};

#endif
