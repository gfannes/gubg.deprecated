#ifndef HEADER_gubg_math_random_Normal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_random_Normal_hpp_ALREADY_INCLUDED

#include "gubg/math/random/DefaultEngine.hpp"
#include "gubg/math/Math.hpp"
#include "gubg/Macro.hpp"
#include <cmath>

#define GUBG_MODULE_ "random::Normal"
#include "gubg/log/begin.hpp"
namespace gubg { namespace math { namespace random { 

    template <typename T>
        T generateNormal(T mean, T sigma)
        {
            return std::normal_distribution<double>(mean, sigma)(defaultEngine);
        }

    double normal_density(double x, double mean, double stddev)
    {
        const double tmp = (x-mean)/stddev;
        return (1.0/(math::Sqrt2PI*stddev))*std::exp(-0.5*tmp*tmp);
    }
    template <typename Ctr>
        void addNormalNoise(Ctr &ctr, double stddev)
        {
            for (auto &v: ctr)
                v += generateNormal(0.0, stddev);
        }

} } } 
#include "gubg/log/end.hpp"

#endif
