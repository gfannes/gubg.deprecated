#ifndef HEADER_gubg_math_distribution_gaussian_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_gaussian_hpp_ALREADY_INCLUDED

#include <vector>
#include <cmath>

#include "gubg/math/stat/Distribution.hpp"
#include "gubg/math/stat/random.hpp"
#include "gubg/math/math.hpp"

using namespace std;

namespace gubg
{
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
            bool draw(DomainT &rnd)
            {
                rnd = Random::drawGaussian(mMean, mSigma);
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
                bool draw(value_type &rnd)
                {
                    if (mSingleValue)
                    {
                        for (int i=0;i<rnd.size();++i)
                            rnd[i] = Random::drawGaussian(mMean, mSigma);
                    } else
                    {
                        rnd.resize(mMeans.size());
                        for (int i=0;i<mMeans.size();++i)
                            rnd[i] = Random::drawGaussian(mMeans[i], mSigmas[i]);
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

#if 0
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
            bool draw(DomainT &value, const DomainT &mean)
            {
                mGaussian.setMean(mean);
                return mGaussian.draw(value);
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
                bool draw(DomainType &value, const DomainType &mean)
                {
                    mGaussian.setMean(mean);
                    return mGaussian.draw(value);
                }
            private:
                GaussianD<DomainType> mGaussian;
        };
#endif
}

#endif
