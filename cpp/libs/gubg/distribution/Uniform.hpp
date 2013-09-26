#ifndef HEADER_gubg_distribution_Uniform_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Uniform_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Engine.hpp"

namespace gubg
{
    namespace distribution
    {
        inline double drawUniform()
        {
            std::uniform_real_distribution<double> rng(0.0, 1.0);
            return rng(gubg::distribution::engine);
        }
        inline bool withProbability(double p)
        {
            return drawUniform() <= p;
        }
        inline double drawUniform(double min, double max)
        {
            return min + drawUniform()*(max-min);
        }
        inline int drawUniform_int(int min, int max)
        {
            std::uniform_int_distribution<int> rng(min, max);
            return rng(gubg::distribution::engine);
        }
    }
}

#endif
