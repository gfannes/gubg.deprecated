#ifndef HEADER_gubg_planning_Day_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Day_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
#include <ostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>

namespace gubg
{
    namespace planning
    {
        class Day
        {
            public:
                Day():y_(0), m_(0), d_(0){}
                Day(int y, int m, int d):y_(y), m_(m), d_(d){}
                Day(const std::string &str):y_(0), m_(0), d_(0)
            {
                Strange strange(str);
                int y, m, d;
                if (strange.popDecimal(y) && strange.popCharIf('-') && strange.popDecimal(m) && strange.popCharIf('-') && strange.popDecimal(d))
                {
                    y_ = y;
                    m_ = m;
                    d_ = d;
                }
            }

                bool isValid() const {return y_ != 0 || m_ != 0 || d_ != 0;}

				int day() const {return d_;}
				int month() const {return m_;}
				int year() const {return y_;}

                Day &operator++()
                {
                    ++d_;
                    switch (m_)
                    {
#ifdef L_CASE
#error L_CASE already defined
#endif
#define L_CASE(m, n)                            case m: if (d_ > n) { d_ = 1; ++m_; } break
                        L_CASE(1, 31);
                        case 2:
                        {
                            int nrDays = (y_%4 == 0 ? 28 : 27);
                            if (y_%100 == 0 && y_%400 != 0)
                                nrDays = 27;
                            if (d_ > nrDays)
                            {
                                d_ = 1;
                                ++m_;
                            }
                        }
                        break;
                        L_CASE(3, 31);
                        L_CASE(4, 30);
                        L_CASE(5, 31);
                        L_CASE(6, 30);
                        L_CASE(7, 31);
                        L_CASE(8, 31);
                        L_CASE(9, 30);
                        L_CASE(10, 31);
                        L_CASE(11, 30);
                        case 12:
                        if (d_ > 31)
                        {
                            d_ = 1;
                            m_ = 1;
                            ++y_;
                        }
                        break;
#undef L_CASE
                    }
                    return *this;
                }

                void stream(std::ostream &os) const
                {
                    std::ostringstream oss;
                    oss << std::setw(4) << std::setfill('0') << y_ << "-" << std::setw(2) << std::setfill('0') << m_ << "-" << std::setw(2) << std::setfill('0') << d_;
                    os << oss.str();
                }

                bool operator<(const Day &rhs) const
                {
                    if (y_ != rhs.y_)
                        return y_ < rhs.y_;
                    if (m_ != rhs.m_)
                        return m_ < rhs.m_;
                    if (d_ != rhs.d_)
                        return d_ < rhs.d_;
                    //Days are equal
                    return false;
                }
                bool operator<=(const Day &rhs) const
                {
                    if (y_ != rhs.y_)
                        return y_ < rhs.y_;
                    if (m_ != rhs.m_)
                        return m_ < rhs.m_;
                    if (d_ != rhs.d_)
                        return d_ < rhs.d_;
                    //Days are equal
                    return true;
                }
                bool operator==(const Day &rhs) const
                {
                    return y_ == rhs.y_ && m_ == rhs.m_ && d_ == rhs.d_;
                }
                bool operator>(const Day &rhs) const {return !operator<=(rhs);}
                bool operator>=(const Day &rhs) const {return !operator<(rhs);}

            private:
                int y_;
                int m_;
                int d_;
        };

		inline Day today()
		{
            auto t = std::time(0);
            auto gt = std::gmtime(&t);
            return Day(gt->tm_year+1900, gt->tm_mon+1, gt->tm_mday);
		}
        typedef std::vector<Day> Days;
        //Returns a vector of nr days starting from today, only including mon ... fri
        inline Days workDays(size_t nr)
        {
            Days days;
            auto t = std::time(0);
            auto gt = std::gmtime(&t);
            Day d(gt->tm_year+1900, gt->tm_mon+1, gt->tm_mday);
            int wday = (gt->tm_wday+6)%7;//wday == 0 => monday
            while (days.size() < nr)
            {
                if (wday < 5)
                    //mon .. fri
                    days.push_back(d);
                ++d;
                wday = (wday+1)%7;
            }
            return days;
        }
        //to is also included
        inline Days dayRange(Day from, Day to)
        {
            Days days;
            while (from <= to)
            {
                days.push_back(from);
                ++from;
            }
            return days;
        }
		//The first quarter might be incomplete
		typedef std::set<Day> Quarter;
		inline std::vector<Quarter> quarters(Day from, const size_t nr)
		{
			std::vector<Quarter> qs;
			Quarter q;
			while (qs.size() != nr)
			{
				q.insert(from);
				++from;
				if (from.day() == 1 && (from.month()%3) == 1)
				{
					qs.push_back(q);
					q.clear();
				}
			}
			return qs;
		}
    }
}
namespace std
{
    inline ostream &operator<<(ostream &os, const gubg::planning::Day &day)
    {
        day.stream(os);
        return os;
    }
}

#endif
