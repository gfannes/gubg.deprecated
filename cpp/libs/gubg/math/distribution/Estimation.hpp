#ifndef HEADER_gubg_math_distribution_Estimation_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_Estimation_hpp_ALREADY_INCLUDED

#include "gubg/math/Codes.hpp"

#define GUBG_MODULE "distribution::Estimation"
#include "gubg/log/begin.hpp"
namespace gubg { namespace math { namespace distribution { 

    template<typename Result, typename Values>
        ReturnCode findMinMax(Result &mi, Result &ma, const Values &values)
        {
            MSS_BEGIN(ReturnCode);
            MSS(!values.empty());
            mi = ma = values.front();
            for (auto value: values)
                if (value < mi)
                    mi = value;
                else if (value > ma)
                    ma = value;
            MSS_END();
        }

    template <typename Mean, typename Values>
        ReturnCode computeMean(Mean &mean, const Values &values)
        {
            MSS_BEGIN(ReturnCode);
            MSS(!values.empty());
            mean = Mean();
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
        ReturnCode computeMeanStdDev(Result &mean, Result &stddev, const Values &values)
        {
            MSS_BEGIN(ReturnCode);
            MSS(values.size() >= 2);
            MSS(computeMean(mean, values));
            stddev = 0.0;
            for (auto value: values)
                stddev += (value-mean)*(value-mean);
            stddev /= values.size()-1;
            stddev = std::sqrt(stddev);
            MSS_END();
        }

    template <typename Result, typename Values, typename Weights>
        ReturnCode computeMeanStdDev(Result &mean, Result &stddev, const Values &values, const Weights &weights)
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

            //Compute stddev
            stddev = 0.0;
            typename Weights::const_iterator weight = weights.begin();
            for (auto value: values)
                stddev += *(weight++)*(value-mean)*(value-mean);
            stddev = std::sqrt(stddev/sumW);

            MSS_END();
        }

} } } 
#include "gubg/log/end.hpp"

#endif
