#ifndef HEADER_gubg_bayesian_Dirichlet_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bayesian_Dirichlet_hpp_ALREADY_INCLUDED

#include "gubg/math/random/Dirichlet.hpp"
#include "gubg/bayesian/Codes.hpp"
#include <array>
#include <ostream>

#define GUBG_MODULE "Dirichlet"
#include "gubg/log/begin.hpp"
namespace gubg { namespace bayesian { 

    template <int Nr, typename Prob>
        class Table;

    template <int Nr, typename Count, typename Prob>
        class Dirichlet
        {
            public:
                typedef Table<Nr, Prob> DataPoint;
                typedef DataPoint ConjugateDataModel;

                Dirichlet()
                {
                    for (auto &count: counts_)
                        count = 1.0;
                }

                void stream(std::ostream &os) const
                {
                    os << "Dirichlet counts (" << counts_.size() << "): ";
                    for (auto count: counts_)
                        os << count << ", ";
                }

                ReturnCode generate(DataPoint &dp) const
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(math::random::generateDirichlet(dp.probs_, counts_));
                    MSS_END();
                }

                ReturnCode observeDataPoint(typename DataPoint::DataPoint dp, Count weight)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(dp >= 0, DataPointTooSmall);
                    MSS(dp < Nr, DataPointTooLarge);
                    counts_[dp] += weight;
                    MSS_END();
                }
                ReturnCode observeDataPoint(typename DataPoint::DataPoint dp)
                {
                    return observeDataPoint(dp, 1.0);
                }

            private:
                typedef std::array<Count, Nr> Counts;
                Counts counts_;
        };

    template <int Nr, typename Count, typename Prob>
        std::ostream &operator<<(std::ostream &os, const Dirichlet<Nr, Count, Prob> &dirichlet)
        {
            dirichlet.stream(os);
            return os;
        }

} } 
#include "gubg/log/end.hpp"

#include "gubg/bayesian/Table.hpp"

#endif
