#ifndef HEADER_gubg_math_distribution_kernel_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_kernel_hpp_ALREADY_INCLUDED

#include <vector>
#include <cmath>

#include "distribution.hpp"
#include "vector.hpp"
#include "math.hpp"
#include "random.hpp"

using namespace std;

namespace gubg
{
// KernelD is a kernel density class that estimates a density based on passed values
    template <typename DomainT = double>
    class KernelD: public Distribution<DomainT>
    {
    public:
        typedef DomainT DomainType;

        KernelD(){}
        double density(double &value) const
            {
                double sum=0.0;
                for (int i=0; i<mData.size(); ++i)
                    sum+=mWeights[i]*scaledKernel(value-mData[i]);
                return sum/mTotalWeight;
            }
        double logDensity(double &value){return log(density(value));}

        bool draw(double &value)
            {
                int ix;
                Random::drawDiscrete(ix, mNormalizedWeights);
                value = Random::drawGaussian(mData[ix], mWidth);
                return true;
            }

        double l1Distance(const KernelD &rhs, long nr = 1000)
            {
                double a,b,x;
                a = mMin - 4.0*mWidth;
                b = rhs.mMin - 4.0*rhs.mWidth;
                double dm = (a < b ? a : b);
                a = mMax + 4.0*mWidth;
                b = rhs.mMax + 4.0*rhs.mWidth;
                double dM = (a > b ? a : b);
                double dD=(dM-dm)/(nr-1);
                Math::computeLinTrans(a, b, 0, dm, nr-1, dM);

                dM = 0.0;
                for (long i=0; i<nr; ++i)
                {
                    Math::linTrans(x, i, a, b);
                    dM += fabs(density(x)-rhs.density(x))*dD;
                }
                return dM;
            }

        bool learn(const vector<DomainT> &data, double *width = NULL, vector<double> *weights = NULL)
            {
                setData(data, weights);
                if (width)
                    setWidth(*width);
                else
                    learnWidth();
                return true;
            }
        bool learnWidth()
            {
                // silvermans rule of thumb for kernel bandwidth selection
                // self.width=(1.06*@data.gStd*(Math.sqrt(@data.length).to_f**(-0.20)))
                // pg 16 of a comparison of kernel density estimates
                // self.width=(1.6644*@data.gStd*(Math.sqrt(@data.length).to_f**(-0.20)))
                double sum, sigma;
                Vector::computeMeanSigma(sum, sigma, mData, mWeights);
                sum *= mData.size();
                if (sigma == 0.0)
                {
                    sigma = 0.01;
                    cerr << "The standard deviation is 0 in KernelD.learnWidth(), I will set it to " << sigma << "" << endl;
                }
                return setWidth(1.6644*sigma*pow(sqrt(sum), -0.50));
            }
        bool setWidth(double width)
            {
                if (width <= 0)
                {
                    cerr << "The width has to be positive for a Kernel distribution" << endl;
                    return false;
                }
                mWidth = width;
                mInvWidth = 1.0/width;
                return true;
            }
        double getWidth(){return mWidth;}
        bool setData(const vector<DomainT> &data, vector<double> *weights = NULL)
            {
                mData = data;
                Vector::findMinMax(mMin, mMax, mData);
                if (weights)
                    mWeights = *weights;
                else
                    Vector::setSame(mWeights, mData.size(), 1.0);
                Vector::computeSum(mTotalWeight, mWeights);
                mNormalizedWeights = mWeights;
                Vector::multiply2All(mNormalizedWeights, 1.0/mTotalWeight);
                return true;            
            }

        static double kernel(double x)
            {
                return 0.398942280401433*exp(-0.5*x*x);
            }
        double scaledKernel(double x) const
            {
                return mInvWidth*kernel(x*mInvWidth);
            }

    private:
        vector<DomainT> mData;
        DomainT mMin;
        DomainT mMax;
        vector<double> mWeights;
        vector<double> mNormalizedWeights;
        double mTotalWeight;
        double mWidth;
        double mInvWidth;
    };
}

#endif
