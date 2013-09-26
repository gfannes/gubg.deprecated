#ifndef HEADER_gubg_distribution_Gaussian_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Gaussian_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Engine.hpp"
#include "gubg/math/Math.hpp"
#include "gubg/Macro.hpp"
#include <cmath>

#define GUBG_MODULE_ "gaussian"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace distribution
    {
        template <typename T>
            T drawGaussian(T mean, T sigma)
            {
                return std::normal_distribution<double>(mean, sigma)(engine);
            }

        double gaussian_prob(double x, double m, double s)
        {
            const double p = (x-m)/s;
            return (1.0/(math::Sqrt2PI*s))*std::exp(-0.5*p*p);
        }
        template <typename Ctr>
            void addGaussianNoise(Ctr &ctr, double s)
            {
                for (auto &v: ctr)
                    v += drawGaussian(0.0, s);
            }
    }
}
#include "gubg/log/end.hpp"

#endif
