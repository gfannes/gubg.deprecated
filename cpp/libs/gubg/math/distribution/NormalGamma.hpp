#ifndef HEADER_gubg_math_distribution_NormalGamma_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_NormalGamma_hpp_ALREADY_INCLUDED

#include <cmath>

namespace gubg { namespace math { namespace distribution { 

    template <typename T>
        class NormalGamma
        {
            public:
                NormalGamma(){}
                NormalGamma(T m, T l, T a, T b): m_(m), l_(l), a_(a), b_(b) {}

                //precision is variance^-1
                T density_unnorm(T mean, T precision) const;

                void mean(T &mean, T &precision) const;

            private:
                T m_;
                T l_;
                T a_;
                T b_;
        };

    //Implementation
    template <typename T>
        T NormalGamma<T>::density_unnorm(T mean, T precision) const
        {
            const T res = std::pow(precision, a_-0.5)*std::exp(-b_*precision - 0.5*(l_*precision*(mean-m_)*(mean-m_)));
            return res;
        }
    template <typename T>
        void NormalGamma<T>::mean(T &mean, T &precision) const
        {
            mean = m_;
            precision = a_/b_;
        }

} } } 
#endif
