#ifndef gubg_bayesian_Learning_hpp
#define gubg_bayesian_Learning_hpp

#include "gubg/bayesian/Codes.hpp"

namespace gubg
{
    namespace bayesian
    {
        template <typename ParamModel, typename DataSet>
            ReturnCode observeDataSet(ParamModel &pm, const DataSet &ds)
            {
                MSS_BEGIN(ReturnCode);
                MSS(NotImplemented);
                for (auto it: ds)
                    MSS(pm.observeDataPoint(it->datapoint(), it->weight()));
                MSS_END();
            }
    }
}

#endif
