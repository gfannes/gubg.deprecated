#ifndef HEADER_gubg_mss_test_Sum_hpp_ALREADY_INCLUDED
#define HEADER_gubg_mss_test_Sum_hpp_ALREADY_INCLUDED

#include "gubg/mss/mss_nostl.hpp"

namespace test
{
    struct Error{};
    enum class ReturnCode {MSS_DEFAULT_CODES, Full};
    struct Sum
    {
        const unsigned long max_;
        unsigned long sum_;

        Sum(unsigned long max):
            max_(max), sum_(0){}

        ReturnCode add_direct(unsigned long v);
        ReturnCode add_mss(unsigned long v);
        void add_exc(unsigned long v);
    };
}

#endif
