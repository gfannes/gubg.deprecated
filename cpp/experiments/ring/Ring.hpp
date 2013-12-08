#ifndef HEADER_gubg_cpp_experiments_ring_Ring_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cpp_experiments_ring_Ring_hpp_ALREADY_INCLUDED

#include "gubg/math/tau.hpp"
#include <array>
#include <ostream>
#include <chrono>

namespace gubg
{
    template <typename T>
        class Value
        {
            public:
                T operator()() const {return v_;}
                void set(T v){v_ = v;}
            private:
                T v_;
        };
}

template <int Nr>
class Ring
{
    public:
        template <typename Stopwatch>
        void process(const Stopwatch &sw)
        {
            static_assert(Stopwatch::duration::period::num == 1, "Cannot use this with a period larger than 1 second");
            double elapse = (double)sw.total_elapse().count()/Stopwatch::duration::period::den;
            for (auto it = values_.begin(); it != values_.end(); ++it)
                it->set(gubg::tau::distance(elapse, (double)(it-values_.begin())/values_.size()));
        }
        void stream(std::ostream &os) const
        {
            for (const auto &v: values_)
            {
                const auto vv = v();
                if (vv < 0.1)
                    os << "     ";
                else if (vv < 0.2)
                    os << "*   ";
                else if (vv < 0.3)
                    os << "**  ";
                else if (vv < 0.4)
                    os << "*** ";
                else if (vv < 0.5)
                    os << "**** ";
            }
        }
    private:
        typedef gubg::Value<double> Value;
        std::array<Value, Nr> values_;
};
template <int Nr>
std::ostream &operator<<(std::ostream &os, const Ring<Nr> &ring)
{
    ring.stream(os);
    return os;
}

#endif
