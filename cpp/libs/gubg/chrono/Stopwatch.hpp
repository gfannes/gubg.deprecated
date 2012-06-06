#ifndef gubg_chrono_Stopwatch_hpp
#define gubg_chrono_Stopwatch_hpp

#include <chrono>

namespace gubg
{
    namespace chrono
    {
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

            private:
                time_point start_;
                time_point prevMark_;
                time_point mark_;
        };
    }
}

#endif
