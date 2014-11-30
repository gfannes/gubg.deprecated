#ifndef HEADER_gubg_data_TableDistribution_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_TableDistribution_hpp_ALREADY_INCLUDED

#include "gubg/data/Nominal.hpp"
#include <array>

namespace gubg { namespace data { 

    template <size_t NrValues, typename T>
        class TableDistribution
        {
            public:
                typedef TableDistribution<NrValues, T> Self;
                typedef std::array<T, NrValues> Probs;

                void clear()
                {
                    counts_.fill(0.0);
                    total_ = 0.0;
                }

                Self &operator+=(const Self &rhs)
                {
                    for (size_t ix = 0; ix < NrValues; ++ix)
                        counts_[ix] += rhs.counts_[ix];
                    total_ += rhs.total_;
                    return *this;
                }

                Self &operator<<(const Nominal<NrValues> &nv)
                {
                    counts_[nv.value()] += 1.0;
                    total_ += 1.0;
                    return *this;
                }

                Probs probs() const
                {
                    Probs probs = counts_;
                    if (total_ != 0.0)
                    {
                        const auto f = 1.0/total_;
                        for (auto &v: probs)
                            v *= f;
                    }
                    return probs;
                }

            private:
                typedef std::array<T, NrValues> Counts;
                Counts counts_{};
                double total_ = 0;
        };

} } 

#endif
