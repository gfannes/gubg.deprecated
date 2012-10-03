#include "gubg/testing/Testing.hpp"
#include "gubg/math/Operations.hpp"
#include <iostream>
using namespace std;
using namespace gubg::math;
#define L(m) cout<<m<<endl

int main()
{
    TEST_TAG(math::Operations);
    unsigned int q, r;
    TEST_CODE(ReturnCode::OnlyUnsignedDivisionForNow, divide(q, r, -16, 4));
    TEST_CODE(ReturnCode::OnlyUnsignedDivisionForNow, divide(q, r, 16, -4));
    TEST_CODE(ReturnCode::DivisionByZero, divide(q, r, 16, 0));
    TEST_OK(divide(q, r, 16, 4));
    L("q: " << q << " r: " << r);
    TEST_EQ(4, q);
    TEST_EQ(0, r);
    return 0;
}
