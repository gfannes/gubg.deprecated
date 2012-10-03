#include "Sum.hpp"

namespace test
{
        ReturnCode Sum::add_direct(unsigned long v)
        {
            if (v > max_ - sum_)
                return ReturnCode::Full;
            sum_ += v;
            return ReturnCode::OK;
        }
        ReturnCode Sum::add_mss(unsigned long v)
        {
            MSS_BEGIN(ReturnCode);
            MSS(v <= max_-sum_, Full);
            sum_ += v;
            MSS_END();
        }
        void Sum::add_exc(unsigned long v)
        {
            if (v > max_ - sum_)
                throw Error();
            sum_ += v;
        }
}
