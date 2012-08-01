#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/FixedVector.hpp"
using namespace gubg;

namespace 
{
    typedef FixedVector<int, 2> FV;
}

int main()
{
    TEST_TAG(main);
    FV fv;
    TEST_EQ(0, fv.size());
    fv.push_back(1);
    TEST_EQ(1, fv.size());
    fv.push_back(2);
    TEST_EQ(2, fv.size());
    fv.push_back(3);
    TEST_EQ(2, fv.size());
    fv.clear();
    TEST_EQ(0, fv.size());
    fv.push_back(1);
    fv.push_back(2);
    TEST_EQ(2, fv.size());

    auto it = fv.begin();
    TEST_EQ(1, *it);
    ++it;
    TEST_EQ(2, *it);
    ++it;
    TEST_EQ(fv.end(), it);

    TEST_EQ(1, fv[0]);
    TEST_EQ(2, fv[1]);
    return 0;
}
