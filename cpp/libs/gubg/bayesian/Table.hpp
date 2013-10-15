#ifndef HEADER_gubg_bayesian_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bayesian_Table_hpp_ALREADY_INCLUDED

#include <array>
#include <ostream>

#define GUBG_MODULE "Table"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace bayesian
    {
        template <int Nr, typename Count, typename Prob>
            class Dirichlet;

        template <int Nr, typename Prob>
            class Table
            {
                public:
                    typedef int DataPoint;

                    Table()
                    {
                        const auto p = 1.0/probs_.size();
                        for (auto &prob: probs_)
                            prob = p;
                    }

                    void stream(std::ostream &os) const
                    {
                        os << "Table probs (" << probs_.size() << "): ";
                        for (auto prob: probs_)
                            os << prob << ", ";
                    }

                private:
                    typedef std::array<Prob, Nr> Probs;
                    Probs probs_;
                    template <int Nr_, typename Count_, typename Prob_>
                        friend class Dirichlet;
            };
    }
}
template <int Nr, typename Prob>
std::ostream &operator<<(std::ostream &os, const gubg::bayesian::Table<Nr, Prob> &table)
{
    table.stream(os);
    return os;
}
#include "gubg/log/end.hpp"

#endif
