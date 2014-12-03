#ifndef HEADER_gubg_data_NominalCounts_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_NominalCounts_hpp_ALREADY_INCLUDED

#include "gubg/data/Nominal.hpp"
#include <array>
#include <ostream>

namespace gubg { namespace data { 

    template <size_t NrValues, typename T>
        class NominalCounts
        {
            public:
                typedef NominalCounts<NrValues, T> Self;
                typedef std::array<T, NrValues> Counts;

                void clear() { counts_.fill(0); }

                Self &operator+=(const Self &rhs)
                {
                    for (size_t ix = 0; ix < NrValues; ++ix)
                        counts_[ix] += rhs.counts_[ix];
                    return *this;
                }

                Self &operator+=(const Nominal<NrValues> &nv)
                {
                    counts_[nv.value()] += 1;
                    return *this;
                }

                Self &operator*=(double factor)
                {
                    for (size_t ix = 0; ix < NrValues; ++ix)
                        counts_[ix] *= factor;
                    return *this;
                }

                Counts counts() const { return counts_; }

                T sum() const
                {
                    T s = 0;
                    for (size_t ix = 0; ix < NrValues; ++ix)
                        s += counts_[ix];
                    return s;
                }

            private:
                Counts counts_{};
        };

    template <size_t NrValues, typename T>
        std::ostream &operator<<(std::ostream &os, const NominalCounts<NrValues, T> &nc)
        {
            const auto counts = nc.counts();
            os << "[";
            for (size_t ix = 0; ix < NrValues; ++ix)
            {
                if (ix > 0)
                    os << ", ";
                os << counts[ix];
            }
            os << "]";
            return os;
        }

} } 

#endif
