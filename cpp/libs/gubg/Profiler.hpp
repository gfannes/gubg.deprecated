#ifndef HEADER_gubg_Profiler_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Profiler_hpp_ALREADY_INCLUDED

#include <array>
#include <chrono>
#include <ostream>

namespace gubg { 

    template <typename Location>
        class Profiler
        {
            public:
                static const size_t Nr = (size_t)Location::Nr;

                typedef Profiler<Location> Self;
                typedef std::chrono::high_resolution_clock Clock;
                typedef unsigned long Elapse;
                typedef std::array<Elapse, Nr> Elapses;

                Profiler(Location location): location_(location), start_(Clock::now()) {}

                const Elapses &elapses() const {return elapses_;}

                void setLocationTo(Location newLocation)
                {
                    const auto newStart = Clock::now();
                    elapses_[(size_t)location_] += (newStart - start_).count();
                    location_ = newLocation;
                    start_ = newStart;
                }

                void stream(std::ostream &os) const
                {
                    for (size_t ix = 0; ix < Nr; ++ix)
                        os << ix << ": " << elapses_[ix] + currentElapse_(ix) << std::endl;
                }

                class RAII
                {
                    public:
                        RAII(Self &self, Location loc): self_(self), loc_(loc) {}
                        ~RAII() { self_.setLocationTo(loc_); }
                    private:
                        Self &self_;
                        const Location loc_;
                };

            private:

                Elapse currentElapse_(size_t ix) const
                {
                    if (ix != (size_t)location_)
                        return 0;
                    return (Clock::now() - start_).count();
                }

                Location location_;
                Clock::time_point start_;

                Elapses elapses_;
        };

    template <typename Location>
    std::ostream &operator<<(std::ostream &os, const Profiler<Location> &prof)
    {
        prof.stream(os);
        return os;
    }

} 

#endif
