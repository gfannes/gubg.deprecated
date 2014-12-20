#ifndef HEADER_gubg_chrono_Stopwatch_hpp_ALREADY_INCLUDED
#define HEADER_gubg_chrono_Stopwatch_hpp_ALREADY_INCLUDED

#include <chrono>
#include <sstream>
#include <iomanip>

namespace gubg { namespace chrono {

    template <typename Clock = std::chrono::high_resolution_clock>
        class Stopwatch
        {
            public:
                typedef Clock clock;
                typedef typename clock::duration duration;
                typedef typename clock::time_point time_point;

                Stopwatch(){reset();}

                Stopwatch &reset(){start_ = prevMark_ = mark_ = clock::now(); return *this;}

                //Observe time, setting a new mark based on current time
                Stopwatch &mark()
                {
                    prevMark_ = mark_;
                    mark_ = clock::now();
                    return *this;
                }

                //Duration between the latest mark() and ctor/reset()
                duration total_elapse() const {return mark_ - start_;}
                //Duration between the latest mark() and the mark() before that
                duration mark_elapse() const {return mark_ - prevMark_;}
                //Last mark time
                time_point mark_time() const {return mark_;}

                //There is a bug in g++ 4.8.1 with O3 with duration_cast, hence we do it ourselves
                template <typename Duration>
                    Duration total_elapse_as() const { return Duration((total_elapse().count()*clock::period::num*Duration::period::den)/clock::period::den/Duration::period::num); }
                template <typename Duration>
                    Duration mark_elapse_as() const { return Duration((mark_elapse().count()*clock::period::num*Duration::period::den)/clock::period::den/Duration::period::num); }

                std::string total_elapse_hr() const
                {
                    std::ostringstream oss;
                    oss << 0.001*total_elapse_as<std::chrono::milliseconds>().count() << " s";
                    return oss.str();
                }

            private:
                time_point start_;
                time_point prevMark_;
                time_point mark_;
        };

} }

#endif
