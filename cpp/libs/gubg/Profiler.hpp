#ifndef HEADER_gubg_Profiler_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Profiler_hpp_ALREADY_INCLUDED

#include <array>
#include <chrono>

namespace gubg { 

    template <typename Locations>
        class Profiler
        {
            public:
                typedef std::chrono::high_resolution_clock Clock;

                Profiler(Location location): location_(location), start_(Clock::now) {}

                void setLocationTo(Location newLocation)
                {
                    const auto newStart_ = Clock::now();
                    elapses_[newLocation_] += (newStart - start_).count();
                    location_ = newLocation;
                    start_ = newStart;
                }

            private:
                Location location_;
                Clock::time_point start_;

                typedef unsigned long Elapse;
                typedef std::array<Elapse, Locations::Nr> Elapses;
                Elapses elapses_;
        };

} 

#endif
