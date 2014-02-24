#ifndef HEADER_gubg_math_distribution_Gamma_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_Gamma_hpp_ALREADY_INCLUDED

namespace gubg { namespace math { namespace distribution { 

    template <typename T>
    class Gamma
    {
        public:
            T density_unnorm(T) const;
            T density_norm(T) const;

            void mean(T &) const;
            void variance(T &) const;
            void stddev(T &) const;

        private:
            T a_;
            T b_;
    };

    //Implementation
    template <typename T>
        T Gamma<T>::density_unnorm(T x) const
        {
            return std::pow(x, a_-1.0)*std::exp(-b_*x);
        }
    template <typename T>
        T Gamma<T>::density_norm(T x) const
        {
            return std::pow(b_, a_)*density_unnorm(x)/std::tgamma(a_);
        }
    template <typename T>
        T Gamma<T>::mean(T &m) const
        {
            m = a_/b_;
        }
    template <typename T>
        T Gamma<T>::variance(T &v) const
        {
            v = a_/b_/b_;
        }
    template <typename T>
        T Gamma<T>::stddev(T &sd) const
        {
            sd = std::sqrt(a_)/b_;
        }

} } } 

#endif
