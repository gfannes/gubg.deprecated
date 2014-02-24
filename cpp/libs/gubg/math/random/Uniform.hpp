#ifndef HEADER_gubg_math_random_Uniform_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_random_Uniform_hpp_ALREADY_INCLUDED

#include "gubg/math/random/DefaultEngine.hpp"

namespace gubg { namespace math { namespace random { 

    inline double drawUniform()
    {
        std::uniform_real_distribution<double> rng(0.0, 1.0);
        return rng(defaultEngine);
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
        return rng(defaultEngine);
    }

} } } 

#endif
