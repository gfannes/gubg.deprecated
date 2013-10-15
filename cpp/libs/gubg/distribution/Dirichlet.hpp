#ifndef HEADER_gubg_distribution_Dirichlet_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Dirichlet_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Codes.hpp"
#include "gubg/distribution/Engine.hpp"
#include <random>

#define GUBG_MODULE "Dirichlet"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace distribution
    {
        template <typename Result, typename Counts>
        ReturnCode generateDirichlet(Result &result, const Counts &counts)
        {
            MSS_BEGIN(ReturnCode);
            MSS(result.size() == counts.size(), SizeMismatch);
            auto res = result.begin();
            typename Result::value_type factor = 0.0;
            for (auto count: counts)
            {
                std::gamma_distribution<typename Result::value_type> gd(count, 1.0);
                auto rn = gd(gubg::distribution::engine);
                factor += (*res++ = rn);
            }
            MSS(factor > 0.0, DivisionByZero);
            factor = 1.0/factor;
            for (auto &p: result)
                p *= factor;
            MSS_END();
        }
    }
}
#include "gubg/log/end.hpp"

#endif
