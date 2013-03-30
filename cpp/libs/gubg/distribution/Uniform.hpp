#ifndef HEADER_gubg_distribution_Uniform_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Uniform_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Engine.hpp"

namespace gubg
{
    namespace distribution
    {
        inline double uniform()
        {
            std::uniform_real_distribution<double> rng(0.0, 1.0);
            return rng(gubg::distribution::engine);
        }
    }
}

#endif
