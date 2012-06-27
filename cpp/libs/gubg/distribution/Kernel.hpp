#ifndef gubg_distribution_Kernel_hpp
#define gubg_distribution_Kernel_hpp

#include "gubg/distribution/Codes.hpp"
#include "gubg/distribution/Table.hpp"
#include "gubg/distribution/Gaussian.hpp"
#include "gubg/distribution/Estimation.hpp"
#include "gubg/function/Linear.hpp"
#include "gubg/vector/vector.hpp"
#include <vector>
#include <cmath>

namespace gubg
{
    namespace distribution
    {
        //Kernel is a kernel density class that estimates a density based on passed values
        template <typename Domain, typename Weight>
            class Kernel
            {
                public:
                    typedef std::vector<Domain> Data;
                    typedef std::vector<Weight> Weights;

                    double density(Domain value) const
                    {
                        LOG_S(density);
                        if (data_.empty())
                            return 0.0;
                        double sum = 0.0;
                        typename Weights::const_iterator weight = weights_.begin();
                        for (auto data: data_)
                        {
                            LOG_M(STREAM(data, *weight));
                            sum += *(weight++)*scaledKernel_(value-data);
                        }
                        LOG_M(sum);
                        return sum/weightDistribution_.sum();
                    }
                    double logDensity(Domain &value){return ::log(density(value));}

                    ReturnCode draw(Domain &value) const
                    {
                        MSS_BEGIN(ReturnCode);
                        size_t ix;
                        MSS(weightDistribution_.generate(ix));
                        value = drawGaussian(data_[ix], width_);
                        MSS_END();
                    }

                    double l1Distance(const Kernel &rhs, long nr = 1000)
                    {
                        double a,b,x;
                        a = min_ - 4.0*width_;
                        b = rhs.min_ - 4.0*rhs.width_;
                        double dm = (a < b ? a : b);
                        a = max_ + 4.0*width_;
                        b = rhs.max_ + 4.0*rhs.width_;
                        double dM = (a > b ? a : b);
                        double dD=(dM-dm)/(nr-1);
                        const function::Linear<double> lt(0, dm, nr-1, dM);

                        dM = 0.0;
                        for (long i=0; i<nr; ++i)
                        {
                            x = lt(i);
                            dM += fabs(density(x)-rhs.density(x))*dD;
                        }
                        return dM;
                    }

                    ReturnCode learn(const Data &data, double *width = nullptr, const Weights *weights = nullptr)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(setData_(data, weights));
                        if (width)
                            MSS(setWidth(*width));
                        else
                            MSS(learnWidth());
                        MSS_END();
                    }
                    ReturnCode learnWidth()
                    {
                        MSS_BEGIN(ReturnCode);
                        //Silvermans rule of thumb for kernel bandwidth selection
                        //self.width=(1.06*@data.gStd*(Math.sqrt(@data.length).to_f**(-0.20)))
                        //pg 16 of a comparison of kernel density estimates
                        //self.width=(1.6644*@data.gStd*(Math.sqrt(@data.length).to_f**(-0.20)))
                        double sum, sigma;
                        MSS(computeMeanSigma(sum, sigma, data_, weights_));
                        sum *= data_.size();
                        MSS(sigma > 0.0, StdDevTooSmall);
                        MSS(setWidth(1.6644*sigma*pow(sqrt(sum), -0.50)));
                        MSS_END();
                    }
                    ReturnCode setWidth(double width)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(width > 0.0, IllegalArgument);
                        width_ = width;
                        invWidth_ = 1.0/width;
                        MSS_END();
                    }
                    double getWidth() const {return width_;}

                private:
                    ReturnCode setData_(const Data &data, const Weights *weights)
                    {
                        MSS_BEGIN(ReturnCode);
                        data_ = data;
                        Vector::findMinMax(min_, max_, data_);
                        if (weights)
                            weights_ = *weights;
                        else
                            weights_ = Weights(data_.size(), 1.0);
                        weightDistribution_.setProbs(weights_);
                        MSS_END();
                    }
                    static double kernel_(double x)
                    {
                        return 0.398942280401433*::exp(-0.5*x*x);
                    }
                    double scaledKernel_(double x) const
                    {
                        return invWidth_*kernel_(x*invWidth_);
                    }

                    Data data_;
                    Domain min_;
                    Domain max_;
                    Weights weights_;
                    Table<Weight> weightDistribution_;
                    double width_;
                    double invWidth_;
            };
    }
}

#endif
