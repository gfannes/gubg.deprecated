#ifndef gubg_distribution_Gaussian_hpp
#define gubg_distribution_Gaussian_hpp

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
