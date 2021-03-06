#ifndef HEADER_gubg_math_stat_Gaussian_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Gaussian_hpp_ALREADY_INCLUDED

//Needs rework or sholud be merged in distribution/Normal.hpp

#include "gubg/math/stat/Distribution.hpp"
#include "gubg/math/random/Normal.hpp"
#include <array>
#include <vector>
#include <initializer_list>
#include <cmath>
using namespace std;

#define GUBG_MODULE "stat::Gaussian"
#include "gubg/log/begin.hpp"
namespace gubg { namespace math { namespace stat { 

    template <typename T>
        class GaussianUV: public Distribution_crtp<GaussianUV<T>, T>
    {
        public:
            GaussianUV &setMean(T m){mean_ = m; return *this;}
            GaussianUV &setSigma(T s){sigma_ = s; return *this;}

        private:
            friend class Distribution_crtp<GaussianUV<T>, T>;
            bool distribution_generate(T &t)
            {
                t = random::generateNormal(mean_, sigma_);
                return true;
            }

            T mean_ = 0;
            T sigma_ = 1;
    };

    template <typename T, size_t Dim>
        class GaussianMV: public Distribution_crtp<GaussianMV<T, Dim>, std::array<T, Dim>>
    {
        public:
            typedef std::array<T, Dim> Mean;
            typedef std::array<T, Dim> Sigma;
            typedef std::array<T, Dim> value_type;

            GaussianMV()
            {
                mean_.fill(0.0);
                sigma_.fill(1.0);
            }

            template <typename M>
                GaussianMV &setMean(const M &m){mean_ = m; return *this;}
            template <typename TT>
                GaussianMV &setMean(std::initializer_list<TT> m){std::copy(m.begin(), m.end(), mean_.begin()); return *this;}
            template <typename S>
                GaussianMV &setSigma(const S &s){sigma_ = s; return *this;}
            template <typename TT>
                GaussianMV &setSigma(std::initializer_list<TT> s){std::copy(s.begin(), s.end(), sigma_.begin()); return *this;}

        private:
            friend class Distribution_crtp<GaussianMV<T, Dim>, std::array<T, Dim>>;
            bool distribution_generate(value_type &t)
            {
                for (size_t i = 0; i < Dim; ++i)
                    t[i] = random::generateNormal(mean_[i], sigma_[i]);
                return true;
            }

            Mean mean_;
            Sigma sigma_;
    };

#if 0
    template <typename DomainT = vector<double> >
        class GaussianD: public Distribution_crtp<GaussianD<DomainT>, DomainT>
    {
        public:
            GaussianD(DomainT mean=0, DomainT sigma=1):
                mMean(mean),
                mSigma(sigma),
                mSigma2(sigma*sigma){};

            double logDensity(DomainT &value)
            {
                return -0.5*(log((Math::PI+Math::PI)*mSigma2)+(value-mMean)*(value-mMean)/mSigma2);
            }
            double gradLogDensity(const DomainT &value)
            {
                return (mMean-value)/mSigma2;
            }
            void setMean(const DomainT &m){mMean = m;};
            bool generate(DomainT &rnd)
            {
                rnd = Random::generateGaussian(mMean, mSigma);
                return true;
            }
        private:
            DomainT mMean;
            DomainT mSigma;
            DomainT mSigma2;
    };

    template <typename T>
        class GaussianD<vector<T>>: public Distribution_crtp<GaussianD<vector<T>>, vector<T>>
        {
            public:
                typedef vector<T> value_type;

                GaussianD(T mean=0, T sigma=1):
                    mSingleValue(true),
                    mMean(mean),
                    mSigma(sigma),
                    mSigma2(sigma*sigma){}
                GaussianD(const value_type &means, const value_type &sigmas):
                    mSingleValue(false),
                    mMeans(means),
                    mSigmas(sigmas),
                    mSigmas2(sigmas)
            {
                for (int i=0;i<mSigmas2.size();++i)
                    mSigmas2[i]*=mSigmas2[i];
            };

                double logDensity(value_type &value)
                {
                    double ld = 0.0;
                    if (mSingleValue)
                    {
                        for (int i=0;i<value.size();++i)
                            ld += -0.5*(log((Math::PI+Math::PI)*mSigma2)+(value[i]-mMean)*(value[i]-mMean)/mSigma2);
                    } else
                    {
                        if (value.size() != mMeans.size())
                            cerr << "Size of value does not correspond with that of the means" << endl;
                        for (int i=0;i<mMeans.size();++i)
                            ld += -0.5*(log((Math::PI+Math::PI)*mSigmas2[i])+(value[i]-mMeans[i])*(value[i]-mMeans[i])/mSigmas2[i]);
                    }
                    return ld;
                }
                bool gradLogDensity(value_type &res, const value_type &values)
                {
                    res.resize(values.size());
                    if (mSingleValue)
                    {
                        for (int i=0; i<values.size(); ++i)
                            res[i] = (mMean-values[i])/mSigma2;
                    } else
                    {
                        if (values.size() != mMeans.size())
                        {
                            cerr << "Size of value does not correspond with that of the means" << endl;
                            return false;
                        }
                        for (int i=0;i<mMeans.size();++i)
                            res[i] = (mMeans[i]-values[i])/mSigmas2[i];
                    }
                    return true; 
                }
                void setMean(const value_type &m){mMeans = m;};
                void setMean(const T &m){mMean = m;};
                bool generate(value_type &rnd)
                {
                    if (mSingleValue)
                    {
                        for (int i=0;i<rnd.size();++i)
                            rnd[i] = Random::generateGaussian(mMean, mSigma);
                    } else
                    {
                        rnd.resize(mMeans.size());
                        for (int i=0;i<mMeans.size();++i)
                            rnd[i] = Random::generateGaussian(mMeans[i], mSigmas[i]);
                    }
                    return true;
                }
            private:
                bool mSingleValue;
                T mMean;
                T mSigma;
                T mSigma2;
                value_type mMeans;
                value_type mSigmas;
                value_type mSigmas2;
        };

    template <typename DomainT = vector<double> >
        class GaussianCD: public ConditionalDistribution<DomainT, DomainT>
    {
        public:
            GaussianCD(DomainT sigma=1):
                mGaussian(0,sigma){}

            double logDensity(DomainT &value, const DomainT &mean)
            {
                mGaussian.setMean(mean);
                return mGaussian.logDensity(value);
            }
            bool generate(DomainT &value, const DomainT &mean)
            {
                mGaussian.setMean(mean);
                return mGaussian.generate(value);
            }
        private:
            GaussianD<DomainT> mGaussian;
    };

    template <typename T>
        class GaussianCD<vector<T> >: public ConditionalDistribution<vector<T>, vector<T> >
        {
            public:
                typedef vector<T> DomainType;

                GaussianCD(T sigma=1):
                    mGaussian(0,sigma){}
                GaussianCD(const DomainType &sigmas):
                    mGaussian(sigmas,sigmas){}

                double logDensity(DomainType &value, const DomainType &mean)
                {
                    mGaussian.setMean(mean);
                    return mGaussian.logDensity(value);
                }
                bool generate(DomainType &value, const DomainType &mean)
                {
                    mGaussian.setMean(mean);
                    return mGaussian.generate(value);
                }
            private:
                GaussianD<DomainType> mGaussian;
        };
#endif

} } } 
#include "gubg/log/end.hpp"

#endif
