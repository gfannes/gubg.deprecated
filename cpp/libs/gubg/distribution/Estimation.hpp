#ifndef gubg_distribution_Estimation_hpp
#define gubg_distribution_Estimation_hpp

#include "gubg/distribution/Codes.hpp"

namespace gubg
{
    namespace distribution
    {
        template <typename Mean, typename Values>
            ReturnCode computeMean(Mean &mean, const Values &values)
            {
                MSS_BEGIN(ReturnCode);
                MSS(!values.empty());
                mean = 0.0;
                for (auto value: values)
                    mean += value;
                mean /= values.size();
                MSS_END();
            }

        template <typename Mean, typename Values, typename Weights>
            ReturnCode computeMean(Mean &mean, const Values &values, const Weights &weights)
            {
                MSS_BEGIN(ReturnCode);
                MSS(!values.empty());
                MSS(values.size() == weights.size());
                mean = 0.0;
                Mean sumW = 0.0;
                typename Values::const_iterator value = values.begin();
                for (auto weight: weights)
                {
                    mean += *(value++)*weight;
                    sumW += weight;
                }
                mean /= sumW;
                MSS_END();
            }

        template <typename Result, typename Values>
            ReturnCode computeMeanSigma(Result &mean, Result &sigma, const Values &values)
            {
                MSS_BEGIN(ReturnCode);
                MSS(values.size() >= 2);
                MSS(computeMean(mean, values));
                sigma = 0.0;
                for (auto value: values)
                    sigma += (value-mean)*(value-mean);
                sigma /= values.size()-1;
                sigma = ::sqrt(sigma);
                MSS_END();
            }

        template <typename Result, typename Values, typename Weights>
            ReturnCode computeMeanSigma(Result &mean, Result &sigma, const Values &values, const Weights &weights)
            {
                MSS_BEGIN(ReturnCode);
                MSS(!values.empty());
                MSS(values.size() == weights.size());

                //Compute mean and the sum of the weights
                mean = 0.0;
                Result sumW = 0.0;
                typename Values::const_iterator value = values.begin();
                for (auto weight: weights)
                {
                    mean += *(value++)*weight;
                    sumW += weight;
                }
                mean /= sumW;

                //Compute sigma
                sigma = 0.0;
                typename Weights::const_iterator weight = weights.begin();
                for (auto value: values)
                    sigma += *(weight++)*(value-mean)*(value-mean);
                sigma = ::sqrt(sigma/sumW);

                MSS_END();
            }
    }
}

#endif
