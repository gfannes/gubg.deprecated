#ifndef HEADER_gubg_distribution_Gaussian_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Gaussian_hpp_ALREADY_INCLUDED

namespace gubg
{
    namespace distribution
    {
        template <typename T>
            T drawGaussian(T mean, T sigma)
            {
                return std::normal_distribution<double>(mean, sigma)(uniform);
            }
    }
}

#endif
