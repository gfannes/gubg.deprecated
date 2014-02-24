#ifndef HEADER_gubg_math_distribution_Normal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_Normal_hpp_ALREADY_INCLUDED

#include "gubg/math/Math.hpp"

namespace gubg { namespace math { namespace distribution { 

    template <typename T>
        class Normal
        {
            public:
                //Setters
                void setMean(T);
                void setVariance(T);
                void setStandardDeviation(T);
                void setPrecision(T);

                T density_unnorm(T x) const;
                T density_norm(T x) const;
                void mean(T &) const;
                void variance(T &) const;
                void standardDeviation(T &) const;
                void precision(T &) const;

            private:
                T m_ = 0.0;
                T sd_ = 1.0;
        };

    //Implementation
    template <typename T>
        T Normal<T>::density_unnorm(T x) const
        {
            const T tmp = (x-m_)/sd_;
            return std::exp(-0.5*tmp*tmp);
        }
    template <typename T>
        T Normal<T>::density_norm(T x) const
        {
            return density_unnorm(x)/(sd_*Sqrt2PI);
        }
    template <typename T>
        void Normal<T>::mean(T &m) const { m = m_; }
    template <typename T>
        void Normal<T>::variance(T &v) const { v = sd_*sd_; }
    template <typename T>
        void Normal<T>::standardDeviation(T &sd) const { sd = sd_; }
    template <typename T>
        void Normal<T>::precision(T &p) const { p = 1.0/(sd_*sd_); }
    //Setters
    template <typename T>
        void Normal<T>::setMean(T m)
        {
            m_ = m;
        }
    template <typename T>
        void Normal<T>::setVariance(T v)
        {
            sd_ = std::sqrt(v);
        }
    template <typename T>
        void Normal<T>::setStandardDeviation(T sd)
        {
            sd_ = sd;
        }
    template <typename T>
        void Normal<T>::setPrecision(T p)
        {
            setVariance(1.0/p);
        }

} } } 

#endif
